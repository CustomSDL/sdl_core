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

struct ifreq;
struct sockaddr;

namespace net {

class LinuxTunAdapter : public TunAdapterInterface {
 public:
  explicit LinuxTunAdapter(const std::string& nic);
  virtual int Create();
  virtual void Destroy(int id);
  virtual std::string GetName(int id) const;
  virtual bool SetAddress(int id, const std::string& value);
  virtual bool GetAddress(int id, std::string *value) const;
  virtual bool SetDestinationAddress(int id, const std::string& value);
  virtual bool GetDestinationAddress(int id, std::string *value) const;
  virtual bool SetNetmask(int id, const std::string& value);
  virtual bool GetNetmask(int id, std::string *value) const;
  virtual bool SetFlags(int id, int value);
  virtual bool GetFlags(int id, int *value) const;
  virtual bool SetMtu(int id, int value);
  virtual bool GetMtu(int id, int *value) const;

 private:
  /**
   * Generates next unique ID for TUN interface
   * @return unique ID
   */
  static int NextId();

  /**
   * Runs command on TUN interface
   * @param cmd command
   * @param ifr structure contains name of TUN interface
   * and is used to give parameter or save result of command
   */
  bool RunCommand(int cmd, ifreq *ifr) const;

  /**
   * Initializes ifr structure and set name of TUN interface
   * @param id unique ID of TUN interface
   * @param ifr structure for initialize
   */
  void InitRequest(int id, ifreq *ifr) const;

  const std::string nic_;
  std::map<int, int> fds_;
};

TunAdapterInterface* CreateTunAdapter(const std::string& nic);
}  // namespace net

#endif  // SRC_COMPONENTS_IVDCM_MODULE_NET_LINUX_TUN_ADAPTER_H_
