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
#include "vr_cooperation/commands/base_json_request.h"
#include "vr_cooperation/event_engine/event_dispatcher.h"
#include "vr_cooperation/vr_module.h"
#include "vr_cooperation/vr_module_constants.h"

namespace vr_cooperation {

namespace commands {

using json_keys::kCode;
using json_keys::kError;
using json_keys::kId;
using json_keys::kMethod;
using json_keys::kParams;
using json_keys::kResult;
using json_keys::kSuccess;
using json_keys::kJsonrpc;

const std::string kJsonRpc = "2.0";

using event_engine::EventDispatcher;

CREATE_LOGGERPTR_GLOBAL(logger_, "VRCooperation")

BaseJsonRequest::BaseJsonRequest(VRModule* parent,
                                 const vr_hmi_api::ServiceMessage& message)
    : parent_(parent),
      gpb_message_(message) {
}

BaseJsonRequest::~BaseJsonRequest() {
}

void BaseJsonRequest::Execute() {
  LOG4CXX_AUTO_TRACE(logger_);
}

vr_hmi_api::ResultCode BaseJsonRequest::GetHMIResultCode(
    const std::string& mobile_code) const {
  vr_hmi_api::ResultCode hmiResCode = vr_hmi_api::INVALID_DATA;

  if (result_codes::kSuccess == mobile_code) {
    hmiResCode = vr_hmi_api::SUCCESS;
  } else if (result_codes::kGenericError == mobile_code) {
    hmiResCode = vr_hmi_api::GENERIC_ERROR;
  } else if (result_codes::kUnsupportedResource == mobile_code) {
    hmiResCode = vr_hmi_api::UNSUPPORTED_RESOURCE;
  } else if (result_codes::kWarnings == mobile_code) {
    hmiResCode = vr_hmi_api::WARNINGS;
  } else if (result_codes::kRejected == mobile_code) {
    hmiResCode = vr_hmi_api::REJECTED;
  } else {
    LOG4CXX_ERROR(logger_, "Unknown Mobile result code ");
    hmiResCode = vr_hmi_api::INVALID_DATA;
  }
  return hmiResCode;
}

void BaseJsonRequest::ParseMobileResultCode(
    const Json::Value& value, vr_hmi_api::ResultCode& result_code) {
  std::string mobile_result_code;
  if (IsMember(value, kResult) && IsMember(value[kResult], kCode)) {
    mobile_result_code = value[kResult][kCode].asString();
  } else if (IsMember(value, kError) && IsMember(value[kError], kCode)) {
    mobile_result_code = value[kError][kCode].asString();
  }

  result_code = GetHMIResultCode(mobile_result_code);
}

void BaseJsonRequest::PrepareRequestMessageForMobile(
    vr_hmi_api::RPCName function_id, const std::string& message_params,
    application_manager::MessagePtr message) {
  LOG4CXX_AUTO_TRACE(logger_);
  Json::Value msg;
  msg[kId] = parent()->GetNextCorrelationID();
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

void BaseJsonRequest::SendMessageToHMI(
    const vr_hmi_api::ServiceMessage& message) {
  LOG4CXX_AUTO_TRACE(logger_);
  parent_->SendMessageToHMI(message);
}

void BaseJsonRequest::SendMessageToMobile(
    application_manager::MessagePtr message) {
  LOG4CXX_AUTO_TRACE(logger_);
  PrepareRequestMessageForMobile(gpb_message_.rpc(), gpb_message_.params(),
                                 message_to_send);
  EventDispatcher<application_manager::MessagePtr, vr_hmi_api::RPCName>::instance()
      ->add_observer(gpb_message_.rpc(), message_to_send->correlation_id(),
                     this);
  LOG4CXX_DEBUG(logger_, "Message to Mob: " << message_to_send->json_message());
  parent_->SendMessageToMobile(message);
}

void BaseJsonRequest::on_event(
    const event_engine::Event<application_manager::MessagePtr,
        vr_hmi_api::RPCName>& event) {
  LOG4CXX_AUTO_TRACE(logger_);
  OnEvent(event);  //runs child's logic

}

}  // namespace commands

}  // namespace vr_cooperation
