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

#include "ivdcm_module/ip_data_sender_receiver.h"

#include "ivdcm_module/ivdcm_proxy.h"
#include "utils/logger.h"

namespace ivdcm_module {

CREATE_LOGGERPTR_GLOBAL(logger_, "IVDCM")

static void OnReceived(u_char *args, const struct pcap_pkthdr *header,
                       const u_char *buffer) {
  IpDataSenderReceiver *receiver = reinterpret_cast<IpDataSenderReceiver*>(args);
  std::vector<uint8_t> data(buffer, buffer + header->caplen);
  receiver->OnMessageReceived(data);
}

class Sniffer : public threads::ThreadDelegate {
 public:
  explicit Sniffer(pcap_t *fd, IpDataSenderReceiver *parent)
      : fd_(fd),
        parent_(parent) {
  }
  virtual void threadMain() {
    LOG4CXX_AUTO_TRACE(logger_);
    int ret = pcap_loop(fd_, -1, &OnReceived,
                        reinterpret_cast<u_char*>(parent_));
    switch (ret) {
      case 0:
        LOG4CXX_INFO(
            logger_,
            "Sniffing loop is finished. Count of packets is exhausted");
        break;
      case -1:
        LOG4CXX_ERROR(logger_, "Could not start sniffing loop");
        break;
      case -2:
        LOG4CXX_INFO(logger_, "Sniffing loop is stopped");
        break;
      default:
        LOG4CXX_ERROR(logger_, "Sniffing loop is finished unexpected");
    }
  }
  virtual void exitThreadMain() {
    LOG4CXX_AUTO_TRACE(logger_);
    pcap_breakloop(fd_);
  }

 private:
  pcap_t *fd_;
  IpDataSenderReceiver *parent_;
};

IpDataSenderReceiver::IpDataSenderReceiver(int id, const std::string& name,
                                           IvdcmProxy *parent)
    : id_(id),
      parent_(parent),
      fd_(0),
      thread_(0) {
  char errbuf[PCAP_ERRBUF_SIZE];
  fd_ = pcap_open_live(name.c_str(), 65535, 0, 10, errbuf);
  if (fd_) {
    thread_ = threads::CreateThread("Sniffer", new Sniffer(fd_, this));
  } else {
    LOG4CXX_ERROR(
        logger_,
        "Could not open TUN interface " << name << ": " << std::string(errbuf));
  }
}

IpDataSenderReceiver::~IpDataSenderReceiver() {
  thread_->join();
  delete thread_->delegate();
  threads::DeleteThread(thread_);
  pcap_close(fd_);
}

bool IpDataSenderReceiver::Start() {
  LOG4CXX_AUTO_TRACE(logger_);
  return thread_->start();
}

void IpDataSenderReceiver::Stop() {
  LOG4CXX_AUTO_TRACE(logger_);
  thread_->stop();
}

void IpDataSenderReceiver::Send(const std::vector<uint8_t>& buff) {
  LOG4CXX_AUTO_TRACE(logger_);
  int ret = pcap_inject(fd_, &(buff)[0], buff.size());
  if (ret == -1) {
    std::string error(pcap_geterr(fd_));
    LOG4CXX_ERROR(logger_, "Could not inject (" << fd_ << "): " << error);
  }
}

void IpDataSenderReceiver::OnMessageReceived(const std::vector<uint8_t>& buff) {
  LOG4CXX_AUTO_TRACE(logger_);
  parent_->OnReceived(id_, buff);
}

}  // namespace ivdcm_module
