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

#include "net/qnx_tun_adapter.h"

#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <errno.h>
#include <net/if.h>
#include <net/if_tun.h>
#include <netinet/in.h>
#include <string>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include "utils/logger.h"

namespace net {

CREATE_LOGGERPTR_GLOBAL(logger_, "IVDCM")

// The network interfaces should be named tun0, tun1, etc.
QnxTunAdapter::QnxTunAdapter()
    : nic_("tun") {
}

bool QnxTunAdapter::RunCommand(int cmd, ifreq *ifr) const {
  LOG4CXX_AUTO_TRACE(logger_);
  int fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (fd == -1) {
    std::string error(strerror(errno));
    LOG4CXX_ERROR(logger_, "Could not create socket with error: " << error);
    return false;
  }
  int ret = ioctl(fd, cmd, ifr);
  if (ret == -1) {
    std::string error(strerror(errno));
    LOG4CXX_ERROR(logger_, "Could not run command with error: " << error);
  }
  close(fd);
  return ret != -1;
}

std::string QnxTunAdapter::GetName(int id) const {
  std::ostringstream name;
  name << nic_ << id;
  return name.str();
}

void QnxTunAdapter::InitRequest(int id, ifreq *ifr) const {
  LOG4CXX_AUTO_TRACE(logger_);
  memset(ifr, 0, sizeof(*ifr));
  std::string name = GetName(id);
  strlcpy(ifr->ifr_name, name.c_str(), sizeof(ifr->ifr_name));
}

/**
 * Converts a string into an Internet address stored in a structure
 * @param value of  Internet address (support only IPv4)
 * @param addr pointer to save result
 */
static void StringToSockAddr(const std::string& value, sockaddr *addr) {
  LOG4CXX_AUTO_TRACE(logger_);
  sockaddr_in *addr_in = (sockaddr_in*) addr;
  addr_in->sin_len = sizeof(*addr_in);
  addr_in->sin_family = AF_INET;
  inet_aton(value.c_str(), &addr_in->sin_addr);
}

int QnxTunAdapter::NextId() {
  static int id = 0;
  return ++id;
}

/**
 * Converts an Internet address into a string
 * @param addr to save result
 * @param value of  Internet address (support only IPv4)
 */
static void SockAddrToString(const sockaddr *addr, std::string *value) {
  LOG4CXX_AUTO_TRACE(logger_);
  sockaddr_in *sin = (sockaddr_in *) addr;
  *value = std::string(inet_ntoa(sin->sin_addr));
}

int QnxTunAdapter::Create() {
  LOG4CXX_AUTO_TRACE(logger_);
  ifreq ifr;
  int id = NextId();
  InitRequest(id, &ifr);
  if (RunCommand(SIOCIFCREATE, &ifr)) {
    return id;
  }
  return -1;
}

void QnxTunAdapter::Destroy(int id) {
  LOG4CXX_AUTO_TRACE(logger_);
  ifreq ifr;
  InitRequest(id, &ifr);
  RunCommand(SIOCIFDESTROY, &ifr);
}

bool QnxTunAdapter::SetAddress(int id, const std::string& value) {
  LOG4CXX_AUTO_TRACE(logger_);
  ifreq ifr;
  InitRequest(id, &ifr);
  StringToSockAddr(value, &ifr.ifr_addr);
  return RunCommand(SIOCSIFADDR, &ifr);
}

bool QnxTunAdapter::GetAddress(int id, std::string* value) const {
  LOG4CXX_AUTO_TRACE(logger_);
  ifreq ifr;
  InitRequest(id, &ifr);
  if (RunCommand(SIOCGIFADDR, &ifr)) {
    SockAddrToString(&ifr.ifr_addr, value);
    return true;
  }
  return false;
}

bool QnxTunAdapter::SetDestinationAddress(int id, const std::string& value) {
  LOG4CXX_AUTO_TRACE(logger_);
  ifreq ifr;
  InitRequest(id, &ifr);
  StringToSockAddr(value, &ifr.ifr_dstaddr);
  return RunCommand(SIOCSIFDSTADDR, &ifr);
}

bool QnxTunAdapter::GetDestinationAddress(int id, std::string* value) const {
  LOG4CXX_AUTO_TRACE(logger_);
  ifreq ifr;
  InitRequest(id, &ifr);
  if (RunCommand(SIOCGIFDSTADDR, &ifr)) {
    SockAddrToString(&ifr.ifr_dstaddr, value);
    return true;
  }
  return false;
}

bool QnxTunAdapter::SetNetmask(int id, const std::string& value) {
  LOG4CXX_AUTO_TRACE(logger_);
  ifreq ifr;
  InitRequest(id, &ifr);
  StringToSockAddr(value, &ifr.ifr_addr);
  return RunCommand(SIOCSIFNETMASK, &ifr);
}

bool QnxTunAdapter::GetNetmask(int id, std::string* value) const {
  LOG4CXX_AUTO_TRACE(logger_);
  ifreq ifr;
  InitRequest(id, &ifr);
  if (RunCommand(SIOCGIFNETMASK, &ifr)) {
    SockAddrToString(&ifr.ifr_addr, value);
    return true;
  }
  return false;
}

bool QnxTunAdapter::SetFlags(int id, int value) {
  LOG4CXX_AUTO_TRACE(logger_);
  ifreq ifr;
  InitRequest(id, &ifr);
  ifr.ifr_flags = value;
  return RunCommand(SIOCSIFFLAGS, &ifr);
}

bool QnxTunAdapter::GetFlags(int id, int* value) const {
  LOG4CXX_AUTO_TRACE(logger_);
  ifreq ifr;
  InitRequest(id, &ifr);
  if (RunCommand(SIOCGIFFLAGS, &ifr)) {
    *value = ifr.ifr_flags;
    return true;
  }
  return false;
}

bool QnxTunAdapter::SetMtu(int id, int value) {
  LOG4CXX_AUTO_TRACE(logger_);
  ifreq ifr;
  InitRequest(id, &ifr);
  ifr.ifr_mtu = value;
  return RunCommand(SIOCSIFMTU, &ifr);
}

bool QnxTunAdapter::GetMtu(int id, int* value) const {
  LOG4CXX_AUTO_TRACE(logger_);
  ifreq ifr;
  InitRequest(id, &ifr);
  if (RunCommand(SIOCGIFMTU, &ifr)) {
    *value = ifr.ifr_mtu;
    return true;
  }
  return false;
}

TunAdapterInterface* CreateTunAdapter(const std::string& nic) {
  // The network interfaces should be named tun0, tun1, etc.
  UNUSED(nic);
  return new QnxTunAdapter();
}

}  // namespace net
