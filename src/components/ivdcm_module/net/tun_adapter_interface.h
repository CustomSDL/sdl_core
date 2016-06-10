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

#ifndef SRC_COMPONENTS_IVDCM_MODULE_NET_TUN_ADAPTER_INTERFACE_H_
#define SRC_COMPONENTS_IVDCM_MODULE_NET_TUN_ADAPTER_INTERFACE_H_

#include <string>

namespace net {

class TunAdapterInterface {
 public:
  virtual ~TunAdapterInterface() {
  }

  /**
   * Creates TUN interface
   * @return -1 if error happened otherwise id of the interface
   */
  virtual int Create() = 0;

  /**
   * Destroys TUN interface
   * @param id of the interface
   */
  virtual void Destroy(int id) = 0;

  /**
   * Sets ifnet address
   * @param id of the interface
   * @param value of ifnet address
   */
  virtual bool SetAddress(int id, const std::string& value) = 0;

  /**
   * Gets ifnet address
   * @param id of the interface
   * @param pointer to save value of ifnet address
   */
  virtual bool GetAddress(int id, std::string *value) = 0;

  /**
   * Sets point-to-point address
   * @param id of the interface
   * @param value of point-to-point address
   */
  virtual bool SetDestinationAddress(int id, const std::string& value) = 0;

  /**
   * Gets point-to-point address
   * @param id of the interface
   * @param pointer to save value of point-to-point address
   */
  virtual bool GetDestinationAddress(int id, std::string *value) = 0;

  /**
   * Sets net mask
   * @param id of the interface
   * @param value of net mask
   */
  virtual bool SetNetmask(int id, const std::string& value) = 0;

  /**
   * Gets net mask
   * @param id of the interface
   * @param pointer to save value of net mask
   */
  virtual bool GetNetmask(int id, std::string *value) = 0;

  /**
   * Sets flags
   * @param id of the interface
   * @param value of flags
   */
  virtual bool SetFlags(int id, int value) = 0;
  /**
   * Gets flags
   * @param id of the interface
   * @param pointer to save value of flags
   */
  virtual bool GetFlags(int id, int *value) = 0;
};

}  // namespace net

#endif  // SRC_COMPONENTS_IVDCM_MODULE_NET_TUN_ADAPTER_INTERFACE_H_
