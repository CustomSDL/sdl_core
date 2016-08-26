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
    : json_message_(message),
      gpb_message_(),
      parent_(parent) {
  service_ = parent_->service();
}

BaseCommandRequest::BaseCommandRequest(
    VRModule* parent,
    const vr_hmi_api::ServiceMessage& message)
    : json_message_(),
      gpb_message_(message),
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

void BaseCommandRequest::on_event(
    const event_engine::Event<application_manager::MessagePtr,
    vr_hmi_api::RPCName>& event) {
  LOG4CXX_AUTO_TRACE(logger_);
  OnEvent(event);  // run child's logic
}

vr_hmi_api::ResultCode BaseCommandRequest::GetHMIResultCode(
    const std::string& mobile_code) const {
  vr_hmi_api::ResultCode hmiResCode = vr_hmi_api::INVALID_DATA;

  if (result_codes::kSuccess == mobile_code) {
    hmiResCode = vr_hmi_api::SUCCESS;
  }
  else if (result_codes::kGenericError == mobile_code) {
    hmiResCode = vr_hmi_api::GENERIC_ERROR;
  }
  else if (result_codes::kUnsupportedResource == mobile_code) {
    hmiResCode = vr_hmi_api::UNSUPPORTED_RESOURCE;
  }
  else if (result_codes::kWarnings == mobile_code) {
    hmiResCode = vr_hmi_api::WARNINGS;
  }
  else if (result_codes::kRejected == mobile_code) {
    hmiResCode = vr_hmi_api::REJECTED;
  }
  else {
    LOG4CXX_ERROR(logger_, "Unknown Mobile result code ");
    hmiResCode = vr_hmi_api::INVALID_DATA;
  }
  return hmiResCode;
}

void BaseCommandRequest::ParseMobileResultCode(const Json::Value& value,
    vr_hmi_api::ResultCode& result_code) {
  std::string mobile_result_code;
  if (IsMember(value, kResult) && IsMember(value[kResult], kCode)) {
    mobile_result_code = value[kResult][kCode].asString();
   } else if (IsMember(value, kError) && IsMember(value[kError], kCode)) {
   mobile_result_code = value[kError][kCode].asString();
  }

  result_code = GetHMIResultCode(mobile_result_code);
}

void BaseCommandRequest::PrepareRequestMessageForMobile(
    vr_hmi_api::RPCName function_id,
    const std::string& message_params,
    application_manager::MessagePtr& message) {
  LOG4CXX_AUTO_TRACE(logger_);
  Json::Value msg;
  msg[kId] = service_->GetNextCorrelationID();
  msg[kJsonrpc] = kJsonRpc;
  msg[kMethod] = function_id;
  if (!message_params.empty()) {
    msg[kParams] = message_params;
  }

  message = new application_manager::Message(
      protocol_handler::MessagePriority::kDefault);
  message->set_protocol_version(application_manager::ProtocolVersion::kV2);
  //TODO(KKarlash): add app id which will be saved in VR Module
  message->set_correlation_id(msg[kId].asInt());
  Json::FastWriter writer;
  message->set_json_message(writer.write(msg));
  message->set_message_type(application_manager::MessageType::kRequest);
}

void BaseCommandRequest::PrepareResponseMessageForMobile(bool success,
    const std::string& result_code,
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
  message->set_protocol_version(application_manager::ProtocolVersion::kV2);
}

void BaseCommandRequest::SendRequestToMobile() {
  LOG4CXX_AUTO_TRACE(logger_);
  application_manager::MessagePtr message_to_send;
  PrepareRequestMessageForMobile(gpb_message_.rpc(),
                                 gpb_message_.params(), message_to_send);
  EventDispatcher<application_manager::MessagePtr,
                  vr_hmi_api::RPCName>::instance()
                  ->add_observer(gpb_message_.rpc(),
                                 message_to_send->correlation_id(), this);
  LOG4CXX_DEBUG(logger_, "Request to Mob: " << message_to_send->json_message());
  service_->SendMessageToMobile(message_to_send);
}

void BaseCommandRequest::SendResponseToMobile(bool success,
                                              const std::string& result_code,
                                              const std::string& info) {
  LOG4CXX_AUTO_TRACE(logger_);

  PrepareResponseMessageForMobile(success, result_code, info, json_message_);
  parent_->SendMessageToMobile(json_message_);
}

void BaseCommandRequest::SendResponseToHMI(
    const vr_hmi_api::ServiceMessage& message) {
  LOG4CXX_AUTO_TRACE(logger_);
  parent_->SendMessageToHMI(message);
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
    int32_t app_id = json_message_->connection_key();
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

