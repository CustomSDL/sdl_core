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

#ifndef SRC_COMPONENTS_IVDCM_MODULE_INCLUDE_IVDCM_MODULE_GPB_DATA_SENDER_RECEIVER_H_
#define SRC_COMPONENTS_IVDCM_MODULE_INCLUDE_IVDCM_MODULE_GPB_DATA_SENDER_RECEIVER_H_

#include <string>
#include "transmitter/transmitter.h"

namespace sdl_ivdcm_api {
class SDLRPC;
}  // namespace sdl_ivdcm_api

namespace net {
class ServerSocket;
}  // namespace net

namespace threads {
class Thread;
}  // namespace threads

namespace ivdcm_module {
class IvdcmProxy;

class GpbDataSenderReceiver {
 public:
  explicit GpbDataSenderReceiver(IvdcmProxy *parent);
  ~GpbDataSenderReceiver();
  bool Start();
  void Stop();
  bool Send(const sdl_ivdcm_api::SDLRPC& message);
  void OnMessageReceived(const std::string &buff);

 private:
  inline void CreateControl();
  inline void DestroyControl();
  inline bool StartControl();
  inline void StopControl();
  inline void CreateTransmit();
  inline void DestroyTransmit();
  inline bool StartTransmit();
  inline void StopTransmit();
  inline bool IsControlMessage(const sdl_ivdcm_api::SDLRPC &message) const;
  IvdcmProxy *parent_;
  transmitter::Transmitter transmitter_;
  net::ServerSocket *socket_;
  threads::Thread* thread_;
  transmitter::Transmitter controller_;
  net::ServerSocket *control_socket_;
  threads::Thread* control_thread_;
  friend class GpbTransmitter;
  friend class ControlTransmitter;
};
}  // namespace ivdcm_module

#endif  // SRC_COMPONENTS_IVDCM_MODULE_INCLUDE_IVDCM_MODULE_GPB_DATA_SENDER_RECEIVER_H_
