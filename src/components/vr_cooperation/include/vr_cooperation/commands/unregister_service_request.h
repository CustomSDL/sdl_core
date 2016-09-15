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

#ifndef SRC_COMPONENTS_VR_COOPERATION_INCLUDE_VR_COOPERATION_COMMANDS_UNREGISTER_SERVICE_REQUEST_H_
#define SRC_COMPONENTS_VR_COOPERATION_INCLUDE_VR_COOPERATION_COMMANDS_UNREGISTER_SERVICE_REQUEST_H_

#include "vr_cooperation/commands/base_gpb_request.h"

namespace vr_cooperation {
class ServiceModule;

namespace commands {

/**
 * @brief UnRegisterServiceRequest command class
 */
class UnregisterServiceRequest : public BaseGpbRequest {
 public:
  /**
   * @brief UnregisterServiceRequest class constructor
   * @param parent pointer to ServiceModule
   * @param message Message from mobile
   **/
  UnregisterServiceRequest(ServiceModule* parent,
                           application_manager::MessagePtr message);

  /**
   * @brief UnregisterServiceRequest class destructor
   */
  virtual ~UnregisterServiceRequest();

  /**
   * @brief Execute command
   */
  virtual void Execute();

  /**
   * @brief This method will be called whenever new event received
   * @param event The received event
   */
  virtual void ProcessEvent(
      const event_engine::Event<vr_hmi_api::ServiceMessage, vr_hmi_api::RPCName>& event);

 private:
  /**
   * @brief send notification to HMI
   */
  void SendNotificationToHMI();

  /**
   * @brief send response message to mobile
   * @param success true if successful; false, if failed
   * @param result Mobile result
   * @param info Provides additional human readable info regarding the result(may be empty)
   */
  void SendResponseToMobile(bool success, const char* result,
                            const std::string& info);
};

}  // namespace commands

}  // namespace vr_cooperation

#endif  // SRC_COMPONENTS_VR_COOPERATION_INCLUDE_VR_COOPERATION_COMMANDS_UNREGISTER_SERVICE_REQUEST_H_
