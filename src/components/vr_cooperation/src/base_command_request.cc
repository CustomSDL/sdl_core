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

#include "utils/logger.h"
#include "vr_cooperation/commands/base_command_request.h"
#include "vr_cooperation/event_engine/event_dispatcher.h"
#include "vr_cooperation/message_helper.h"
#include "vr_cooperation/vr_module.h"
#include "vr_cooperation/vr_module_constants.h"
#include "vr_cooperation/interface/hmi.pb.h"

namespace vr_cooperation {

namespace commands {

using event_engine::EventDispatcher;

using json_keys::kSuccess;
using json_keys::kResultCode;
using json_keys::kInfo;
using json_keys::kId;
using json_keys::kParams;
using json_keys::kJsonrpc;
using json_keys::kMethod;
using json_keys::kResult;
using json_keys::kCode;
using json_keys::kError;
using json_keys::kMessage;
using json_keys::kAppId;

const std::string kJsonRpc = "2.0";

CREATE_LOGGERPTR_GLOBAL(logger_, "VRCooperation")

BaseCommandRequest::BaseCommandRequest(
    VRModule* parent,
    const application_manager::MessagePtr& message)
    : message_(message),
      parent_(parent) {
  service_ = parent_->service();
}

BaseCommandRequest::~BaseCommandRequest() {
}


void BaseCommandRequest::OnTimeout() {
}

void BaseCommandRequest::Init() {
  LOG4CXX_AUTO_TRACE(logger_);
}

void BaseCommandRequest::Run() {
  LOG4CXX_AUTO_TRACE(logger_);
  Execute();  // run child's logic
}

void BaseCommandRequest::on_event(const event_engine::Event<application_manager::MessagePtr,
    std::string>& event) {
  LOG4CXX_AUTO_TRACE(logger_);
  OnEvent(event);  // run child's logic
}

const hmi_apis::Common_Result::eType BaseCommandRequest::GetHMIResultCode(
    const std::string& mobile_code) const {
  hmi_apis::Common_Result::eType hmiResCode = hmi_apis::Common_Result::GENERIC_ERROR;

  if (result_codes::kSuccess == mobile_code) {
    hmiResCode = hmi_apis::Common_Result::SUCCESS;
  }
  else if (result_codes::kUnsupportedRequest == mobile_code) {
    hmiResCode = hmi_apis::Common_Result::UNSUPPORTED_REQUEST;
  }
  else if (result_codes::kUnsupportedResource == mobile_code) {
    hmiResCode = hmi_apis::Common_Result::UNSUPPORTED_RESOURCE;
  }
  else if (result_codes::kDisallowed == mobile_code) {
    hmiResCode = hmi_apis::Common_Result::DISALLOWED;
  }
  else if (result_codes::kRejected == mobile_code) {
    hmiResCode = hmi_apis::Common_Result::REJECTED;
  }
  else if (result_codes::kAborted == mobile_code) {
    hmiResCode = hmi_apis::Common_Result::ABORTED;
  }
  else if (result_codes::kIgnored == mobile_code) {
    hmiResCode = hmi_apis::Common_Result::IGNORED;
  }
  else if (result_codes::kRetry == mobile_code) {
    hmiResCode = hmi_apis::Common_Result::RETRY;
  }
  else if (result_codes::kInUse == mobile_code) {
    hmiResCode = hmi_apis::Common_Result::IN_USE;
  }
  else if (result_codes::kVehicleDataNotAvailable == mobile_code) {
    hmiResCode = hmi_apis::Common_Result::DATA_NOT_AVAILABLE;
  }
  else if (result_codes::kTimedOut == mobile_code) {
    hmiResCode = hmi_apis::Common_Result::TIMED_OUT;
  }
  else if (result_codes::kInvalidData == mobile_code) {
    hmiResCode = hmi_apis::Common_Result::INVALID_DATA;
  }
  else if (result_codes::kCharLimitExceeded == mobile_code) {
    hmiResCode = hmi_apis::Common_Result::CHAR_LIMIT_EXCEEDED;
  }
  else if (result_codes::kInvalidId == mobile_code) {
    hmiResCode = hmi_apis::Common_Result::INVALID_ID;
  }
  else if (result_codes::kDuplicateName == mobile_code) {
    hmiResCode = hmi_apis::Common_Result::DUPLICATE_NAME;
  }
  else if (result_codes::kApplicationNotRegistered == mobile_code) {
    hmiResCode = hmi_apis::Common_Result::APPLICATION_NOT_REGISTERED;
  }
  else if (result_codes::kWrongLanguage == mobile_code) {
    hmiResCode = hmi_apis::Common_Result::WRONG_LANGUAGE;
  }
  else if (result_codes::kOutOfMemory == mobile_code) {
    hmiResCode = hmi_apis::Common_Result::OUT_OF_MEMORY;
  }
  else if (result_codes::kTooManyPendingRequests == mobile_code) {
    hmiResCode = hmi_apis::Common_Result::TOO_MANY_PENDING_REQUESTS;
  }
  else if (result_codes::kApplicationNotRegistered == mobile_code) {
    hmiResCode = hmi_apis::Common_Result::NO_APPS_REGISTERED;
  }
  else if (result_codes::kApplicationNotRegistered == mobile_code) {
    hmiResCode = hmi_apis::Common_Result::NO_DEVICES_CONNECTED;
  }
  else if (result_codes::kWarnings == mobile_code) {
    hmiResCode = hmi_apis::Common_Result::WARNINGS;
  }
  else if (result_codes::kGenericError == mobile_code) {
    hmiResCode = hmi_apis::Common_Result::GENERIC_ERROR;
  }
  else if (result_codes::kUserDisallowed == mobile_code) {
    hmiResCode = hmi_apis::Common_Result::USER_DISALLOWED;
  }
  else {
    LOG4CXX_ERROR(logger_, "Unknown Mobile result code ");
    hmiResCode = hmi_apis::Common_Result::GENERIC_ERROR;
  }
  return hmiResCode;
}

bool BaseCommandRequest::ParseMobileResultCode(const Json::Value& value,
    int& result_code,
    std::string& info) {
  result_code = -1;
  info = "";

  std::string mobile_result_code;
  if (IsMember(value, kResult) && IsMember(value[kResult], kCode)) {
    mobile_result_code = value[kResult][kCode].asString();
   } else if (IsMember(value, kError) && IsMember(value[kError], kCode)) {
   mobile_result_code = value[kError][kCode].asString();
  }

  result_code = GetHMIResultCode(mobile_result_code);
  if (IsMember(value[kError], kMessage)) {
    info = value[kError][kMessage].asString();
  }

  // TODO (KKarlash) compare result_code with HMI result code values

  return result_code != -1;
}

void BaseCommandRequest::PrepareRequestMessageForMobile(const char* function_id,
    const Json::Value& message_params,
    application_manager::MessagePtr& message) {
  LOG4CXX_AUTO_TRACE(logger_);
  Json::Value msg;
  msg[kId] = service_->GetNextCorrelationID();
  msg[kJsonrpc] = kJsonRpc;
  msg[kMethod] = function_id;
  if (!message_params.isNull()) {
    msg[kParams] = message_params;
  }

  message = new application_manager::Message(
      protocol_handler::MessagePriority::kDefault);
  message->set_protocol_version(application_manager::ProtocolVersion::kV2);
  message->set_connection_key(message_->connection_key());
  message->set_correlation_id(msg[kId].asInt());
  Json::FastWriter writer;
  message->set_json_message(writer.write(msg));
  message->set_message_type(application_manager::MessageType::kRequest);
}

void BaseCommandRequest::PrepareResponseMessageForHMI(bool success,
    const int& result_code,
    const std::string& info,
    application_manager::MessagePtr& message) {
  LOG4CXX_AUTO_TRACE(logger_);
  Json::Value msg_params;
  msg_params[kSuccess] = success;
  msg_params[kResultCode] = result_code;
  if (!info.empty()) {
    msg_params[kInfo] = info;
  }

  message->set_message_type(application_manager::MessageType::kResponse);
  Json::FastWriter writer;
  message->set_json_message(writer.write(msg_params));
  message->set_correlation_id(msg_params[kId].asInt());
  message->set_protocol_version(application_manager::ProtocolVersion::kHMI);
}

void BaseCommandRequest::PrepareResponseMessageForMobile(bool success,
    const int& result_code,
    const std::string& info,
    application_manager::MessagePtr& message) {
  LOG4CXX_AUTO_TRACE(logger_);
  Json::Value msg_params;
  msg_params[kId] = service_->GetNextCorrelationID();
  msg_params[kSuccess] = success;
  msg_params[kResultCode] = result_code;
  if (!info.empty()) {
    msg_params[kInfo] = info;
  }

  message->set_message_type(application_manager::MessageType::kResponse);
  Json::FastWriter writer;
  message->set_json_message(writer.write(msg_params));
  message->set_correlation_id(msg_params[kId].asInt());
  message->set_function_id(message_->function_id());
  message->set_connection_key(message_->connection_key());
  message->set_protocol_version(application_manager::ProtocolVersion::kV2);
}


void BaseCommandRequest::SendRequest(const char* function_id,
    const Json::Value& message_params,
    bool is_hmi_request/* = false*/) {
  LOG4CXX_AUTO_TRACE(logger_);

  if (is_hmi_request) {
    application_manager::MessagePtr message_to_send;
    PrepareRequestMessageForMobile(function_id, message_params, message_to_send);
    EventDispatcher<application_manager::MessagePtr, std::string>::instance()->
        add_observer(function_id, message_to_send->correlation_id(), this);
    LOG4CXX_DEBUG(logger_, "Request to HMI: " << message_to_send->json_message());
    service_->SendMessageToMobile(message_to_send);
  }
}

void BaseCommandRequest::SendResponse(bool success,
    const int& result_code,
    const std::string& info,
    bool is_mob_response/* = false*/) {
  LOG4CXX_AUTO_TRACE(logger_);
  if (is_mob_response) {
    PrepareResponseMessageForHMI(success, result_code, info, message_);
    parent_->SendMessageToHMI(message_);
  } else {
    PrepareResponseMessageForMobile(success, result_code, info, message_);
    parent_->SendMessageToMobile(message_);
  }
}

void BaseCommandRequest::SendNotification(
    bool is_hmi_notification/* = false*/) {
  LOG4CXX_AUTO_TRACE(logger_);

  if (is_hmi_notification) {
    vr_hmi_api::ServiceMessage service_message;
    service_message.set_rpc(vr_hmi_api::ON_REGISTER);
    service_message.set_rpc_type(vr_hmi_api::NOTIFICATION);
    service_message.set_correlation_id(service_->GetNextCorrelationID());
    vr_hmi_api::OnRegisterServiceNotification onregister_notification;
    int32_t app_id = message_->connection_key();
    // TODO(Thinh): Uncomment after vr module GPB functionality implementation
//    app_id == parent_->default_app_id() ?
//        onregister_notification.set_default(true) :
    onregister_notification.set_default_(false);
    onregister_notification.set_appid(app_id);

    std::string str;
    onregister_notification.SerializeToString(&str);

    service_message.set_params(str);
    // TODO(Thinh): Uncomment after vr module GPB functionality implementation
//    parent_->SendMessageToHMI(service_message);
  }
}

}  // namespace commands

}  // namespace vr_cooperation

