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

#ifndef SRC_COMPONENTS_IVDCM_MODULE_NET_LINUX_TUN_ADAPTER_H_
#define SRC_COMPONENTS_IVDCM_MODULE_NET_LINUX_TUN_ADAPTER_H_

#include <map>
#include <string>

#include "net/tun_adapter_interface.h"

namespace net {

class LinuxTunAdapter : public TunAdapterInterface {
 public:
  explicit LinuxTunAdapter(const std::string& nic);
  virtual int Create();
  virtual void Destroy(int id);
  virtual bool SetAddress(int id, const std::string& value);
  virtual bool GetAddress(int id, std::string *value);
  virtual bool SetDestinationAddress(int id, const std::string& value);
  virtual bool GetDestinationAddress(int id, std::string *value);
  virtual bool SetNetmask(int id, const std::string& value);
  virtual bool GetNetmask(int id, std::string *value);
  virtual bool SetFlags(int id, int value);
  virtual bool GetFlags(int id, int *value);
  virtual bool SetMtu(int id, int value);
  virtual bool GetMtu(int id, int *value);

 private:
  static int NextId();
  bool RunCommand(int cmd, ifreq *ifr) const;
  void InitRequest(int id, ifreq *ifr) const;

  /**
   * Converts a string into an Internet address stored in a structure
   * @param value of  Internet address (support only IPv4)
   * @param addr pointer to save result
   */
  void StringToSockAddr(const std::string& value, sockaddr *addr) const;

  /**
   * Converts an Internet address into a string
   * @param addr to save result
   * @param value of  Internet address (support only IPv4)
   */
  void SockAddrToString(const sockaddr *addr, std::string *value) const;

  const std::string nic_;
  std::map<int, int> fds_;
};

}  // namespace net

#endif  // SRC_COMPONENTS_IVDCM_MODULE_NET_LINUX_TUN_ADAPTER_H_
