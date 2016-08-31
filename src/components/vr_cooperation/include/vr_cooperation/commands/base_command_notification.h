/*
 Copyright (c) 2016, Ford Motor Company
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 Redistributions of source code must retain the above copyright notice, this
 list of conditions and the following disclaimer.

 Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following
 disclaimer in the documentation and/or other materials provided with the
 distribution.

 Neither the name of the Ford Motor Company nor the names of its contributors
 may be used to endorse or promote products derived from this software
 without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef SRC_COMPONENTS_VR_COOPERATION_INCLUDE_VR_COOPERATION_COMMANDS_BASE_COMMAND_NOTIFICATION_H_
#define SRC_COMPONENTS_VR_COOPERATION_INCLUDE_VR_COOPERATION_COMMANDS_BASE_COMMAND_NOTIFICATION_H_

#include "application_manager/service.h"
#include "functional_module/function_ids.h"
#include "utils/macro.h"
#include "vr_cooperation/interface/hmi.pb.h"
#include "vr_cooperation/commands/command.h"

namespace Json {
class Value;
}

namespace vr_cooperation {
class VRModule;

namespace commands {

/**
 * @brief Base command class for notifications
 */
class BaseCommandNotification : public Command {
 public:
  /**
   * @brief BaseCommandNotification class constructor
   * @param parent pointer to VRModule
   * @param message Message from HMI
   */
  BaseCommandNotification(VRModule* parent,
                          const vr_hmi_api::ServiceMessage& message);

  /**
   * @brief BaseCommandNotification class destructor
   */
  virtual ~BaseCommandNotification();

  /**
   * @brief BaseCommandNotification on timeout reaction
   */
  virtual void OnTimeout() {}

  /**
   * @brief run request
   */
  virtual void Run();

 protected:
  vr_hmi_api::ServiceMessage message() {
    return message_;
  }

  VRModule* parent() {
    return parent_;
  }

  /**
   * @brief send notification to Mobile
   * @param function_id mobile function id
   * @param msg_params json with message params
   */
  void SendNotification(functional_modules::MobileFunctionID function_id,
                        const Json::Value& msg_params);

  /**
   * @brief Interface method that executes specific logic of children classes
   */
  virtual void Execute() = 0;

 private:
  vr_hmi_api::ServiceMessage message_;
  VRModule* parent_;
  application_manager::ServicePtr service_;
};

}  // namespace commands

}  // namespace vr_cooperation

#endif  // SRC_COMPONENTS_VR_COOPERATION_INCLUDE_VR_COOPERATION_COMMANDS_BASE_COMMAND_NOTIFICATION_H_
