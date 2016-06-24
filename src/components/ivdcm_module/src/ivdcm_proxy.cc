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

#include <arpa/inet.h>
#include <net/if.h>
#include <netinet/if_ether.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <sstream>

#include "config_profile/profile.h"
#include "ivdcm_module/ivdcm_proxy_listener.h"
#include "net/tun_adapter.h"
#include "utils/logger.h"

namespace ivdcm_module {

CREATE_LOGGERPTR_GLOBAL(logger_, "IVDCM")

struct ShowPacketInfo {
  explicit ShowPacketInfo(const std::vector<uint8_t>* packet)
    : packet_(packet) {
  }
  const std::vector<uint8_t>* packet_;
};
std::ostream& operator << (std::ostream& output, const ShowPacketInfo& info) {
  output << "length(" << info.packet_->size() << ")";
  const ether_header *eth_hdr = reinterpret_cast<const ether_header*>(&info.packet_[0]);
#ifdef __QNXNTO__
  const int eth_hdr_len = ETHER_HDR_LEN;
#else
  const int eth_hdr_len = ETH_HLEN;
#endif
  const ip *ip_hdr = reinterpret_cast<const ip*>(&info.packet_[eth_hdr_len]);
  int header_len = (int)(ip_hdr->ip_hl*4);
  const tcphdr *tcp_hdr = reinterpret_cast<const tcphdr*>(&info.packet_[eth_hdr_len + header_len]);
  output << std::hex;
  output << " source(" << eth_hdr->ether_shost << ")";
  output << " destination(" << eth_hdr->ether_dhost << ")";
  output << " type(" << ntohs(eth_hdr->ether_type) << ")";
  output << " IP version(" << (unsigned int) ip_hdr->ip_v << ")";
  output << " IP Header length(" << (unsigned int) ip_hdr->ip_hl << ")";
  output << " IP tos (" << (uint8_t) ip_hdr->ip_tos << ")";
  output << " IP Packet length (" << (uint16_t) ntohs(ip_hdr->ip_len) << ")";
  output << " IP Id (" << (uint16_t) ntohs(ip_hdr->ip_id) << ")";
  output << " IP offset (" << (uint16_t) ntohs(ip_hdr->ip_off) << ")";
  output << " IP TTL (" << (uint8_t) ip_hdr->ip_ttl << ")";
  output << " IP protocol (" << (uint8_t) ip_hdr->ip_p << ")";
  output << " IP checksum (" << (uint16_t) ntohs(ip_hdr->ip_sum) << ")";
  output << std::dec;
  output << " IP src_addr (" << inet_ntoa(ip_hdr->ip_src) << ")";
  output << " IP dst_addr (" << inet_ntoa(ip_hdr->ip_dst) << ")";
  uint8_t dest_port, source_port;
#ifdef __QNXNTO__
    source_port = ntohs(tcp_hdr->th_sport);
    dest_port = ntohs(tcp_hdr->th_dport);
#else
    source_port = ntohs(tcp_hdr->source);
    dest_port = ntohs(tcp_hdr->dest);
#endif
  output << " source port (" << (uint16_t) source_port << ")";
  output << " destination (" << (uint16_t) dest_port << ")";
  return output;
}

IvdcmProxy::IvdcmProxy(IvdcmProxyListener *listener)
    : listener_(listener),
      gpb_(this),
      ip_data_sender_(),
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
    ip_data_sender_.AddTun(id, tun_->GetName(id));
  }
  return id;
}

void IvdcmProxy::DestroyTun(int id) {
  LOG4CXX_AUTO_TRACE(logger_);
  ip_data_sender_.RemoveTun(id);
  tun_->Destroy(id);
}

std::string IvdcmProxy::NextIp() const {
  LOG4CXX_AUTO_TRACE(logger_);
  static int i = 0;
  const int kMax = 255;
  if (++i > kMax) {
    LOG4CXX_ERROR(logger_, "Range of IP addresses is exhausted");
    return "";
  }
  std::stringstream ip;
  ip << std::string(ip_range_.begin(), ip_range_.end() - 1) << i;
  return ip.str();
}

std::string IvdcmProxy::GetAddressTun(int id) {
  LOG4CXX_AUTO_TRACE(logger_);
  std::string ip;
  if (tun_->GetAddress(id, &ip)) {
    return ip;
  }
  return "";
}

std::string IvdcmProxy::GetNameTun(int id) {
  LOG4CXX_AUTO_TRACE(logger_);
  return tun_->GetName(id);
}

void IvdcmProxy::Send(int id, const std::vector<uint8_t>& ip_data) {
  LOG4CXX_AUTO_TRACE(logger_);
  LOG4CXX_DEBUG(logger_, "IP packet: " << ShowPacketInfo(&ip_data));
  ip_data_sender_.Send(id, ip_data);
}

}  // namespace ivdcm_module

