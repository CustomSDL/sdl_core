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
#include "vr_cooperation/message_helper.h"
#include "vr_cooperation/service_module.h"
#include "vr_cooperation/vr_module_constants.h"

namespace vr_cooperation {

namespace commands {

using json_keys::kAppId;
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

BaseJsonRequest::BaseJsonRequest(ServiceModule* parent,
                                 const vr_hmi_api::ServiceMessage& message)
    : parent_(parent),
      gpb_message_(message) {
}

BaseJsonRequest::~BaseJsonRequest() {
}

void BaseJsonRequest::OnTimeout() {
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

functional_modules::MobileFunctionID BaseJsonRequest::GetMobileFunctionID(
    vr_hmi_api::RPCName function_id) {
  switch (function_id) {
    case vr_hmi_api::ON_REGISTER:
      return functional_modules::REGISTER_SERVICE;
    case vr_hmi_api::ACTIVATE:
      return functional_modules::ACTIVATE_SERVICE;
    case vr_hmi_api::ON_DEACTIVATED:
      return functional_modules::ON_SERVICE_DEACTIVATED;
    case vr_hmi_api::PROCESS_DATA:
      return functional_modules::PROCESS_DATA;
    default:
      LOG4CXX_ERROR(logger_, "Unknown Mobile function id ");
      return functional_modules::VR_UNKNOWN;
  }
}

void BaseJsonRequest::ParseMobileResultCode(
    const Json::Value& value, vr_hmi_api::ResultCode& result_code) {
  std::string mobile_result_code;
  if (IsMember(value, kResult) && IsMember(value[kResult], kCode)) {
    mobile_result_code = value[kResult][kCode].asString();
  } else if (IsMember(value, kError) && IsMember(value[kError], kCode)) {
    mobile_result_code = value[kError][kCode].asString();
  } else {
    LOG4CXX_ERROR(logger_, "Unknown Mobile result code ");
    mobile_result_code = result_codes::kInvalidData;
  }

  result_code = GetHMIResultCode(mobile_result_code);
}

void BaseJsonRequest::SendMessageToHMI(
    const vr_hmi_api::ServiceMessage& message) {
  LOG4CXX_AUTO_TRACE(logger_);
  parent_->SendMessageToHMI(message);
}

void BaseJsonRequest::SendMessageToMobile(
    application_manager::MessagePtr message) {
  LOG4CXX_AUTO_TRACE(logger_);

  message->set_protocol_version(application_manager::ProtocolVersion::kV2);
  message->set_correlation_id(gpb_message_.correlation_id());
  message->set_function_id(GetMobileFunctionID(gpb_message_.rpc()));

  message->set_message_type(application_manager::MessageType::kRequest);
  EventDispatcher<application_manager::MessagePtr, vr_hmi_api::RPCName>::instance()
      ->add_observer(gpb_message_.rpc(), message->correlation_id(), this);
  LOG4CXX_DEBUG(logger_, "Message to Mob: " << message->json_message());
  parent_->SendMessageToMobile(message);
}

void BaseJsonRequest::on_event(
    const event_engine::Event<application_manager::MessagePtr,
        vr_hmi_api::RPCName>& event) {
  LOG4CXX_AUTO_TRACE(logger_);
  ProcessEvent(event);  //runs child's logic
}

}  // namespace commands

}  // namespace vr_cooperation
