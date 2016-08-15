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

#ifndef SRC_COMPONENTS_VR_COOPERATION_INCLUDE_VR_COOPERATION_COMMANDS_BASE_COMMAND_REQUEST_H_
#define SRC_COMPONENTS_VR_COOPERATION_INCLUDE_VR_COOPERATION_COMMANDS_BASE_COMMAND_REQUEST_H_

#include "application_manager/message.h"
#include "application_manager/service.h"

#include "interfaces/HMI_API.h"

#include "vr_cooperation/commands/command.h"
#include "vr_cooperation/event_engine/event_observer.h"
#include "vr_cooperation/vr_module.h"
#include "json/json.h"

namespace vr_cooperation {

class VRModule;

namespace commands {

/**
 * @brief Base command class for requests
 */
class BaseCommandRequest : public Command,
<<<<<<< 069d222a1de8630fd1a3b656d743032923bc0e01
  public event_engine::EventObserver<application_manager::MessagePtr, std::string> {
=======
  public event_engine::EventObserver<
  application_manager::MessagePtr, const std::string&> {
>>>>>>> REVSDL-2384 RegisterService request implementation
 public:
  /**
   * @brief BaseCommandRequest class constructor
   * @param vr_module parent VRModule
   * @param message Message from mobile
   **/
  explicit BaseCommandRequest(VRModule* parent,
                              const application_manager::MessagePtr& message);

  /**
   * @brief BaseCommandRequest class destructor
   */
  virtual ~BaseCommandRequest();

  /**
   * @brief BaseCommandRequest on timeout reaction
   */
  virtual void OnTimeout();

  /**
   * @brief initialize request
   */
  virtual void Init();

  /**
   * @brief run request
   */
  virtual void Run();

  /**
   * @brief This method will be called when receive an event
   *
   * @param event The received event
   */
  virtual void on_event(const event_engine::Event<application_manager::MessagePtr,
      std::string>& event);

 protected:
  /**
   * @brief Converts Mobile string result code to HMI code
   * @param mobile_code Mobile result code
   * @return eType value with HMI result code
   */
  const hmi_apis::Common_Result::eType GetHMIResultCode(
      const std::string& mobile_code) const;

  /**
   * @brief Parse result code from response
   *
   * @param value message in response from Mobile
   * @param result_code Outgoing param with result code
   * @param info Outgoing param with additional human readable info regarding the result(may be empty)
   * @return true if it is success response? otherwise false
   */
  bool ParseMobileResultCode(const Json::Value& value,
                             int&  result_code,
                             std::string& info);

  /**
   * @brief Prepares request message for Mobile
   * @param function_id request ID
   * @param msg_params json with message params
   * @param message that will be sent to mobile
   */
  void PrepareRequestMessageForMobile(const char* function_id,
      const Json::Value& message_params,
      application_manager::MessagePtr& message);

  /**
   * @brief Prepares response message for HMI
   * @param success true if successful; false, if failed
   * @param result_code Mobile result codes
   * @param info Provides additional human readable info regarding the result(may be empty)
   * @param message that will be sent to HMI
   */
  void PrepareResponseMessageForHMI(bool success,
                                    const int& result_code,
                                    const std::string& info,
                                    application_manager::MessagePtr& message);

  /**
   * @brief send request to HMI or Mobile
   * @param function_id request ID
   * @param msg_params json with message params
   * @param is_hmi_request send request to hmi or mobile
   */
  void SendRequest(const char* function_id,
                   const Json::Value& message_params,
                   bool is_hmi_request = false);

  /**
   * @brief Send response to HMI or Mobile
   * @param success true if successful; false, if failed
   * @param result_code Mobile result codes
   * @param info Provides additional human readable info regarding the result(may be empty)
   * @param is_mob_response true response for mobile; false - for HMI
   */
  void SendResponse(bool success,
                    const int& result_code,
                    const std::string& info,
                    bool is_mob_response = false);

  /**
   * @brief send notification to HMI
   * @param function_id request ID
   * @param msg_params json with message params
   */
  void SendNotification(const char* function_id,
                        const Json::Value& message_params);

  /**
   * @brief Interface method that is called whenever new event received
   */
  virtual void OnEvent(const event_engine::Event<application_manager::MessagePtr,
      std::string>& event) = 0;

  /**
   * @brief Interface method that executes specific logic of children classes
   */
  virtual void Execute() = 0;

  application_manager::MessagePtr message_;
  VRModule* parent_;

 private:
  application_manager::ServicePtr service_;
  application_manager::ApplicationSharedPtr app_;
};

}  // namespace commands

}  // namespace vr_cooperation

#endif  // SRC_COMPONENTS_VR_COOPERATION_INCLUDE_VR_COOPERATION_COMMANDS_BASE_COMMAND_REQUEST_H_
