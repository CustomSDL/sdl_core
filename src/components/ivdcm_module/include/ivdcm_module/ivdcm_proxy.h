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

#ifndef SRC_COMPONENTS_IVDCM_MODULE_INCLUDE_IVDCM_MODULE_IVDCM_PROXY_H_
#define SRC_COMPONENTS_IVDCM_MODULE_INCLUDE_IVDCM_MODULE_IVDCM_PROXY_H_

#include <string>
#include "ivdcm_module/gpb_data_sender_receiver.h"

namespace net {
class TunAdapterInterface;
}  // namespace net

namespace ivdcm_module {
class IvdcmProxyListener;

class IvdcmProxy {
 public:
  explicit IvdcmProxy(IvdcmProxyListener *listener);
  ~IvdcmProxy();
  bool Send(const sdl_ivdcm_api::SDLRPC &message);
  void OnReceived(const sdl_ivdcm_api::SDLRPC &message);
  int CreateTun();
  void DestroyTun(int id);

  /**
   * Gets address of tunnel software network interface (TUN)
   * @param id unique number of TUN
   * @return empty string if error is occurred otherwise ip address
   */
  std::string GetAddressTun(int id);

 private:
  std::string NextIp() const;
  IvdcmProxyListener *listener_;
  GpbDataSenderReceiver gpb_;
  std::string ip_range_;
  net::TunAdapterInterface *tun_;
};
}  // namespace ivdcm_module

#endif  // SRC_COMPONENTS_IVDCM_MODULE_INCLUDE_IVDCM_MODULE_IVDCM_PROXY_H_
