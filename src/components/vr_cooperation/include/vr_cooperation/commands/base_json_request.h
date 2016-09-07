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

#ifndef SRC_COMPONENTS_VR_COOPERATION_INCLUDE_VR_COOPERATION_COMMANDS_BASE_JSON_REQUEST_H_
#define SRC_COMPONENTS_VR_COOPERATION_INCLUDE_VR_COOPERATION_COMMANDS_BASE_JSON_REQUEST_H_

#include "vr_cooperation/commands/base_command_request.h"
#include "vr_cooperation/event_engine/event_observer.h"
#include "json/json.h"

namespace vr_cooperation {

class VRModule;

namespace commands {

/**
 * @brief Base command class for json requests
 */
class BaseJsonRequest : public BaseCommandRequest,
    public event_engine::EventObserver<application_manager::MessagePtr,
        vr_hmi_api::RPCName> {
 public:
  /**
   * @brief BaseJsonRequest class constructor
   * @param parent pointer to VRModule
   * @param message Message from HMI
   */
  BaseJsonRequest(VRModule* parent, const vr_hmi_api::ServiceMessage& message);

  /**
   * @brief BaseJsonRequest class destructor
   */
  virtual ~BaseJsonRequest();

  /**
   * @brief BaseJsonRequest on timeout reaction
   */
  virtual void OnTimeout();

  /**
   * @brief This method will be called when receive an event
   *
   * @param event The received event
   */
  virtual void on_event(
      const event_engine::Event<application_manager::MessagePtr,
          vr_hmi_api::RPCName>& event);

 protected:
  /**
   * @brief Converts Mobile string result code to HMI code
   * @param mobile_code Mobile result code
   * @return ResultCode value with HMI result code
   */
  vr_hmi_api::ResultCode GetHMIResultCode(const std::string& mobile_code) const;

  /**
   * @brief Interface method that is called whenever new event received
   */
  virtual void OnEvent(
      const event_engine::Event<application_manager::MessagePtr,
          vr_hmi_api::RPCName>& event) = 0;

  /**
   * @brief Parse result code from response
   *
   * @param value message in response from Mobile
   * @param result_code Outgoing param with result code
   */
  void ParseMobileResultCode(const Json::Value& value,
                             vr_hmi_api::ResultCode& result_code);

  /**
   * @brief Prepares request message for Mobile
   * @param function_id request ID
   * @param msg_params params from gpb message
   * @param message that will be sent to mobile
   */
  void PrepareRequestMessageForMobile(vr_hmi_api::RPCName function_id,
                                      const std::string& message_params,
                                      application_manager::MessagePtr& message);

  /**
   * @brief send message (request/response) to HMI
   * @param message gpb message for HMI
   */
  virtual void SendMessageToHMI(const vr_hmi_api::ServiceMessage& message);

  /**
   * @brief send message (request/response) to HMI
   * @param message json message for Mobile
   */
  virtual void SendMessageToMobile(
      const application_manager::MessagePtr& message);

  /**
   * @brief Interface method that executes specific logic of children classes
   */
  virtual void Execute();

 protected:
  /**
   * @brief Returns parent
   */
  VRModule* parent() const {
    return parent_;
  }

 private:
  VRModule* parent_;
  vr_hmi_api::ServiceMessage gpb_message_;
};

}  // namespace commands

}  // namespace vr_cooperation

#endif // SRC_COMPONENTS_VR_COOPERATION_INCLUDE_VR_COOPERATION_COMMANDS_BASE_JSON_REQUEST_H_
