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

CREATE_LOGGERPTR_GLOBAL(logger_, "VRCooperation")

BaseCommandRequest::BaseCommandRequest(
  const application_manager::MessagePtr& message)
  : message_(message) {
  service_ = VRModule::instance()->service();
  app_ = service_->GetApplication(message_->connection_key());
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

void BaseCommandRequest::on_event() {
  LOG4CXX_AUTO_TRACE(logger_);
}

void BaseCommandRequest::SendResponse(bool success,
    const char* result_code,
    const std::string& info,
    bool is_mob_response/* = false*/) {
  LOG4CXX_AUTO_TRACE(logger_);
  Json::Value msg_params;


  msg_params[kSuccess] = success;
  msg_params[kResultCode] = result_code;
  if (!info.empty()) {
    msg_params[kInfo] = info;
  }
  Json::FastWriter writer;
  std::string params = writer.write(msg_params);
  message_->set_json_message(params);
  message_->set_protocol_version(
        application_manager::ProtocolVersion::kHMI);
  if (is_mob_response) {
    VRModule::instance()->SendResponseToHMI(message_);
  }
}

void BaseCommandRequest::SendRequest(const char* function_id,
    const Json::Value& message_params,
    bool is_hmi_request/* = false*/) {
  LOG4CXX_AUTO_TRACE(logger_);
  Json::Value msg;

  if (is_hmi_request) {
    msg[kId] = service_->GetNextCorrelationID();
  }

  EventDispatcher<application_manager::MessagePtr, std::string>::instance()->
  add_observer(function_id, msg[kId].asInt(), this);

  msg[kJsonrpc] = "2.0";
  msg[kMethod] = function_id;
  if (!message_params.isNull()) {
    msg[kParams] = message_params;
  }

  msg[kParams][json_keys::kAppId] = app_->hmi_app_id();

  Json::FastWriter writer;
  if (is_hmi_request) {
    application_manager::MessagePtr message_to_send(
      new application_manager::Message(
        protocol_handler::MessagePriority::kDefault));
    message_to_send->set_protocol_version(
      application_manager::ProtocolVersion::kV2);
    message_to_send->set_correlation_id(msg[kId].asInt());
    std::string json_msg = writer.write(msg);
    message_to_send->set_json_message(json_msg);
    message_to_send->set_message_type(
      application_manager::MessageType::kRequest);

    LOG4CXX_DEBUG(logger_, "Request to HMI: " << json_msg);
    service_->SendMessageToHMI(message_to_send);
  }

}

}  // namespace commands

}  // namespace vr_cooperation

