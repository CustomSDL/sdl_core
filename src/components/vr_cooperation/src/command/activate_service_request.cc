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

#include "functional_module/function_ids.h"
#include "vr_cooperation/commands/activate_service_request.h"
#include "vr_cooperation/service_module.h"
#include "vr_cooperation/vr_module_constants.h"
#include "utils/logger.h"

namespace vr_cooperation {

namespace commands {
const int kVoiceRecognition = 0;

CREATE_LOGGERPTR_GLOBAL(logger_, "ActivateServiceRequest")

ActivateServiceRequest::ActivateServiceRequest(
      ServiceModule* parent,
      const vr_hmi_api::ServiceMessage& message)
  : BaseJsonRequest(parent, message) {
}

ActivateServiceRequest::~ActivateServiceRequest() {
}

void ActivateServiceRequest::Execute() {
  LOG4CXX_AUTO_TRACE(logger_);
  vr_hmi_api::ActivateServiceRequest params;
  if (gpb_message().has_params() && params.ParseFromString(gpb_message().params())) {
    int32_t app_id = params.appid();
    parent()->ActivateService(app_id);
  }
  application_manager::MessagePtr message_to_send =
      new application_manager::Message(protocol_handler::MessagePriority::kDefault);
  message_to_send->set_message_type(application_manager::MessageType::kRequest);
  SendMessageToMobile(message_to_send);
}

void ActivateServiceRequest::ProcessEvent(
    const event_engine::Event<application_manager::MessagePtr, int32_t>& event) {
  LOG4CXX_AUTO_TRACE(logger_);
  Json::Value value;
  Json::Reader reader;
  reader.parse(event.event_message()->json_message(), value);

  vr_hmi_api::ServiceMessage message_to_hmi;
  PrepareGpbMessage(value, message_to_hmi);
  SendMessageToHMI(message_to_hmi);
}

std::string ActivateServiceRequest::GetParams(
    const Json::Value& value) {
  LOG4CXX_AUTO_TRACE(logger_);
  vr_hmi_api::ResultCode result_code;
  ParseMobileResultCode(value, result_code);
  vr_hmi_api::ActivateServiceResponse response;
  response.set_result(result_code);
  if (vr_hmi_api::SUCCESS != result_code) {
    parent()->DeactivateService();
  }
  std::string params;
  response.SerializeToString(&params);

  return params;
}

void ActivateServiceRequest::PrepareGpbMessage(
    const Json::Value& value,
    vr_hmi_api::ServiceMessage& message) {
  LOG4CXX_AUTO_TRACE(logger_);
  message.set_rpc(vr_hmi_api::ACTIVATE);
  message.set_rpc_type(vr_hmi_api::RESPONSE);
  message.set_correlation_id(parent()->GetNextCorrelationID());
  message.set_params(GetParams(value));
}

}  // namespace commands

}  // namespace vr_cooperation
