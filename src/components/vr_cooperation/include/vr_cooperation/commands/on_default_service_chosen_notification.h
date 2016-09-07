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

#ifndef SRC_COMPONENTS_VR_COOPERATION_INCLUDE_VR_COOPERATION_COMMANDS_ON_DEFAULT_SERVICE_CHOSEN_NOTIFICATION_H_
#define SRC_COMPONENTS_VR_COOPERATION_INCLUDE_VR_COOPERATION_COMMANDS_ON_DEFAULT_SERVICE_CHOSEN_NOTIFICATION_H_

#include <string>
#include "vr_cooperation/commands/json_notification.h"

namespace vr_hmi_api {
class ServiceMessage;
}  // namespace vr_hmi_api

namespace vr_cooperation {

class VRModule;

namespace commands {

/**
 * @brief OnDefaultServiceChosenNotification command class
 */
class OnDefaultServiceChosenNotification : public JsonNotification {
 public:
  /**
   * @brief OnDefaultServiceChosenNotification class constructor
   * @param parent pointer to VRModule
   * @param message Message from HMI
   */
  OnDefaultServiceChosenNotification(VRModule* parent,
                                     const vr_hmi_api::ServiceMessage& message);

  /**
   * @brief OnDefaultServiceChosenNotification class destructor
   */
  virtual ~OnDefaultServiceChosenNotification();

  /**
   * @brief execute command
   */
  virtual void Execute();
};

}  // namespace commands

}  // namespace vr_cooperation

#endif  // SRC_COMPONENTS_VR_COOPERATION_INCLUDE_VR_COOPERATION_COMMANDS_ON_DEFAULT_SERVICE_CHOSEN_NOTIFICATION_H_
