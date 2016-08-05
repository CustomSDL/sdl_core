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

#ifndef SRC_COMPONENTS_VR_COOPERATION_INCLUDE_VR_COOPERATION_COMMANDS_HMI_REQUEST_FROM_HMI_H_
#define SRC_COMPONENTS_VR_COOPERATION_INCLUDE_VR_COOPERATION_COMMANDS_HMI_REQUEST_FROM_HMI_H_

#include "vr_cooperation/commands/command.h"
#include "application_manager/message.h"
#include "utils/macro.h"

namespace vr_cooperation {

namespace commands {

class RequestFromHMI: public Command {
 public:
  /**
   * @brief RequestFromHMI class constructor
   * @param message Message from mobile
   **/
  explicit RequestFromHMI(const application_manager::MessagePtr& message);

  /**
   * @brief RequestFromHMI class destructor
   */
  virtual ~RequestFromHMI();

  /**
   * @brief on timeout reaction
   */
  virtual void OnTimeout();

  /**
   * @brief run command
   */
  virtual void Run();

  /**
   * @brief Interface method that is called whenever new event received
   */
  virtual void on_event();

 protected:
  /**
   * @brief Sends request to mobile
   */
  void SendRequest();
  /**
   * @brief SendResponse allows to send response to hmi
   */
  void SendResponse();
  /**
   * @brief executes specific logic of children classes
   */
  void virtual Execute() = 0;

  /**
   * @brief Interface method that is called whenever new event received
   */
  void virtual OnEvent() = 0;

  application_manager::MessagePtr message_;

 private:
  DISALLOW_COPY_AND_ASSIGN(RequestFromHMI);
};

}  // namespace commands
}  // namespace vr_cooperation

#endif  // SRC_COMPONENTS_VR_COOPERATION_INCLUDE_VR_COOPERATION_COMMANDS_HMI_REQUEST_FROM_HMI_H_
