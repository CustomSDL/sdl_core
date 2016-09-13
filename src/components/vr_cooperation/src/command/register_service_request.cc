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

#include "vr_cooperation/commands/register_service_request.h"

#include "utils/logger.h"
#include "json/json.h"
#include "vr_cooperation/command_factory.h"
#include "vr_cooperation/vr_module.h"
#include "vr_cooperation/vr_module_constants.h"

namespace vr_cooperation {

namespace commands {

using json_keys::kId;
using json_keys::kInfo;
using json_keys::kResultCode;
using json_keys::kSuccess;

CREATE_LOGGERPTR_GLOBAL(logger_, "VRCooperation")

RegisterServiceRequest::RegisterServiceRequest(
    VRModule* parent, application_manager::MessagePtr message)
    : BaseGpbRequest(parent, message) {
}

RegisterServiceRequest::~RegisterServiceRequest() {
}

void RegisterServiceRequest::Execute() {
  LOG4CXX_AUTO_TRACE(logger_);
  std::string result = result_codes::kSuccess;
  bool success = true;
  std::string info;

  SendResponseToMobile(success, result.c_str(), info);
  if (success) {
    SendNotificationToHMI();
  }
}

void RegisterServiceRequest::ProcessEvent(
    const event_engine::Event<vr_hmi_api::ServiceMessage, vr_hmi_api::RPCName>& event) {
  LOG4CXX_AUTO_TRACE(logger_);
}

void RegisterServiceRequest::SendNotificationToHMI() {
  LOG4CXX_AUTO_TRACE(logger_);
  application_manager::MessagePtr json_msg = json_message();
  json_msg->set_message_type(application_manager::MessageType::kNotification);
  commands::Command* command = CommandFactory::Create(parent(), json_msg);
  if (command) {
    command->Run();
    delete command;
  }
}

void RegisterServiceRequest::SendResponseToMobile(bool success,
                                                  const char* result,
                                                  const std::string& info) {
  LOG4CXX_AUTO_TRACE(logger_);
  Json::Value msg_params;
  msg_params[kId] = parent()->service()->GetNextCorrelationID();
  msg_params[kSuccess] = success;
  msg_params[kResultCode] = result;
  msg_params[kInfo] = info;

  application_manager::MessagePtr json_msg = json_message();
  json_msg->set_message_type(application_manager::MessageType::kResponse);
  Json::FastWriter writer;
  json_msg->set_json_message(writer.write(msg_params));
  json_msg->set_correlation_id(msg_params[kId].asInt());
  json_msg->set_protocol_version(application_manager::ProtocolVersion::kV2);
  parent()->SendMessageToMobile(json_msg);
}

}  // namespace commands

}  // namespace vr_cooperation
