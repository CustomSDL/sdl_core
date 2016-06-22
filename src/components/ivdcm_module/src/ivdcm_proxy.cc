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

#include "ivdcm_module/ivdcm_proxy.h"

#include <net/if.h>
#include <sstream>

#include "config_profile/profile.h"
#include "ivdcm_module/ivdcm_proxy_listener.h"
#include "net/tun_adapter.h"
#include "utils/logger.h"

namespace ivdcm_module {

CREATE_LOGGERPTR_GLOBAL(logger_, "IVDCM")

IvdcmProxy::IvdcmProxy(IvdcmProxyListener *listener)
    : listener_(listener),
      gpb_(GpbDataSenderReceiver(this)),
      ip_range_(),
      tun_(0) {
  ip_range_ = profile::Profile::instance()->ivdcm_ip_range();
  std::string nic = profile::Profile::instance()->ivdcm_nic_name();
  tun_ = net::CreateTunAdapter(nic);
  gpb_.Start();
}

IvdcmProxy::~IvdcmProxy() {
  gpb_.Stop();
  delete tun_;
}

bool IvdcmProxy::Send(const sdl_ivdcm_api::SDLRPC &message) {
  LOG4CXX_AUTO_TRACE(logger_);
  return gpb_.Send(message);
}

void IvdcmProxy::OnReceived(const sdl_ivdcm_api::SDLRPC &message) {
  LOG4CXX_AUTO_TRACE(logger_);
  listener_->OnReceived(message);
}

int IvdcmProxy::CreateTun() {
  LOG4CXX_AUTO_TRACE(logger_);
  int id = tun_->Create();
  if (id != -1) {
    tun_->SetAddress(id, NextIp());
    tun_->SetDestinationAddress(id, NextIp());
    tun_->SetNetmask(id, "255.255.255.0");
    tun_->SetFlags(id, IFF_UP | IFF_NOARP);
    uint16_t mtu = profile::Profile::instance()->ivdcm_mtu();
    tun_->SetMtu(id, mtu);
  }
  return id;
}

void IvdcmProxy::DestroyTun(int id) {
  LOG4CXX_AUTO_TRACE(logger_);
  tun_->Destroy(id);
}

std::string IvdcmProxy::NextIp() const {
  LOG4CXX_AUTO_TRACE(logger_);
  static int i = 0;
  const int kMax = 255;
  if (++i <= kMax) {
    std::stringstream s;
    s << i;
    std::string number;
    s >> number;
    std::string ip = ip_range_;
    ip.replace(ip.end() - 1, ip.end(), number);
    return ip;
  } else {
    LOG4CXX_ERROR(logger_, "Range of IP addresses is exhausted");
    return "";
  }
}

std::string IvdcmProxy::GetAddressTun(int id) {
  LOG4CXX_AUTO_TRACE(logger_);
  std::string ip;
  if (tun_->GetAddress(id, &ip)) {
    return ip;
  }
  return "";
}

}  // namespace ivdcm_module

