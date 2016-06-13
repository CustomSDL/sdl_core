/*
 * Copyright (c) 2016, Ford Motor Company
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following
 * disclaimer in the documentation and/or other materials provided with the
 * distribution.
 *
 * Neither the name of the Ford Motor Company nor the names of its contributors
 * may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "ivdcm_module/gpb_data_sender_receiver.h"

#include "config_profile/profile.h"
#include "ivdcm_module/interface/rpc.pb.h"
#include "ivdcm_module/ivdcm_proxy.h"
#include "net/connected_socket_impl.h"
#include "net/server_socket_impl.h"
#include "utils/threads/thread.h"
#include "utils/threads/thread_delegate.h"

namespace ivdcm_module {

CREATE_LOGGERPTR_GLOBAL(logger_, "IVDCM")

class GpbTransmitter : public threads::ThreadDelegate {
 public:
  explicit GpbTransmitter(GpbDataSenderReceiver *parent)
      : parent_(parent) {
  }
  virtual void threadMain() {
    LOG4CXX_AUTO_TRACE(logger_);
    bool stop = false;
    while (!stop) {
      net::ConnectedSocket *conn = parent_->socket_->accept();
      LOG4CXX_INFO(logger_, "Incoming connection was accepted");
      if (!conn) break;
      parent_->transmitter_.set_socket(conn);
      stop = parent_->transmitter_.MessageListenLoop(parent_);
      parent_->transmitter_.set_socket(NULL);
      conn->close();
      delete conn;
    }
  }
  virtual void exitThreadMain() {
    LOG4CXX_AUTO_TRACE(logger_);
    parent_->transmitter_.Stop();
  }
 private:
  GpbDataSenderReceiver *parent_;
};

class ControlTransmitter : public threads::ThreadDelegate {
 public:
  explicit ControlTransmitter(GpbDataSenderReceiver *parent)
      : parent_(parent) {
  }
  virtual void threadMain() {
    LOG4CXX_AUTO_TRACE(logger_);
    bool stop = false;
    while (!stop) {
      net::ConnectedSocket *conn = parent_->control_socket_->accept();
      LOG4CXX_INFO(logger_, "Incoming connection was accepted");
      if (!conn) break;
      parent_->controller_.set_socket(conn);
      stop = parent_->controller_.MessageListenLoop(parent_);
      parent_->controller_.set_socket(NULL);
      conn->close();
      delete conn;
    }
  }
  virtual void exitThreadMain() {
    LOG4CXX_AUTO_TRACE(logger_);
    parent_->controller_.Stop();
  }
 private:
  GpbDataSenderReceiver *parent_;
};

GpbDataSenderReceiver::GpbDataSenderReceiver(IvdcmProxy *parent)
    : parent_(parent),
      transmitter_(),
      socket_(0),
      thread_(0),
      controller_(),
      control_socket_(0),
      control_thread_(0) {
  CreateControl();
  CreateTransmit();
}

GpbDataSenderReceiver::~GpbDataSenderReceiver() {
  Stop();
  DestroyTransmit();
  DestroyControl();
}

void GpbDataSenderReceiver::CreateControl() {
  LOG4CXX_AUTO_TRACE(logger_);
  std::string ip = profile::Profile::instance()->ivdcm_ip();
  uint32_t control_port = profile::Profile::instance()->ivdcm_control_port();
  control_socket_ = new net::ServerSocketImpl(ip.c_str(), control_port);
  control_thread_ = threads::CreateThread("IvdcmGpbControlTransmitter",
                                          new ControlTransmitter(this));
}

void GpbDataSenderReceiver::CreateTransmit() {
  LOG4CXX_AUTO_TRACE(logger_);
  std::string ip = profile::Profile::instance()->ivdcm_ip();
  uint32_t port = profile::Profile::instance()->ivdcm_port();
  socket_ = new net::ServerSocketImpl(ip.c_str(), port);
  thread_ = threads::CreateThread("IvdcmGpbTransmitter",
                                  new GpbTransmitter(this));
}

void GpbDataSenderReceiver::DestroyControl() {
  LOG4CXX_AUTO_TRACE(logger_);
  control_thread_->join();
  delete control_thread_->delegate();
  threads::DeleteThread(control_thread_);
  control_socket_->close();
  delete control_socket_;
}

void GpbDataSenderReceiver::DestroyTransmit() {
  LOG4CXX_AUTO_TRACE(logger_);
  thread_->join();
  delete thread_->delegate();
  threads::DeleteThread(thread_);
  socket_->close();
  delete socket_;
}

bool GpbDataSenderReceiver::StartTransmit() {
  LOG4CXX_AUTO_TRACE(logger_);
  const int kOneClient = 1;
  return socket_->bind() && socket_->listen(kOneClient) && thread_->start();
}

bool GpbDataSenderReceiver::StartControl() {
  LOG4CXX_AUTO_TRACE(logger_);
  const int kOneClient = 1;
  return control_socket_->bind() && control_socket_->listen(kOneClient)
      && control_thread_->start();
}

bool GpbDataSenderReceiver::Start() {
  LOG4CXX_AUTO_TRACE(logger_);
  return StartControl() && StartTransmit();
}

void GpbDataSenderReceiver::StopTransmit() {
  LOG4CXX_AUTO_TRACE(logger_);
  thread_->stop();
  socket_->shutdown();
}

void GpbDataSenderReceiver::StopControl() {
  LOG4CXX_AUTO_TRACE(logger_);
  control_thread_->stop();
  control_socket_->shutdown();
}

void GpbDataSenderReceiver::Stop() {
  LOG4CXX_AUTO_TRACE(logger_);
  StopTransmit();
  StopControl();
}

bool GpbDataSenderReceiver::IsControlMessage(
    const sdl_ivdcm_api::SDLRPC &message) const {
  return message.rpc_name()
      == sdl_ivdcm_api::SDLRPC_SDLRPCName::SDLRPC_SDLRPCName_ON_INTERNET_STATE;
}

bool GpbDataSenderReceiver::Send(const sdl_ivdcm_api::SDLRPC &message) {
  LOG4CXX_AUTO_TRACE(logger_);
  std::string buff;
  bool ret = message.SerializeToString(&buff);
  if (ret) {
    if (IsControlMessage(message)) {
      return controller_.Send(buff);
    } else {
      return transmitter_.Send(buff);
    }
  } else {
    LOG4CXX_WARN(logger_, "Could not serialize GPB message");
    return false;
  }
}

void GpbDataSenderReceiver::OnMessageReceived(const std::string &buff) {
  LOG4CXX_AUTO_TRACE(logger_);
  sdl_ivdcm_api::SDLRPC message;
  bool ret = message.ParseFromString(buff);
  if (ret) {
    parent_->OnReceived(message);
  } else {
    LOG4CXX_WARN(logger_, "Could not parse GPB message");
  }
}

}  // namespace ivdcm_module
