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

#include "vr_cooperation/commands/base_gpb_request.h"

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

BaseGpbRequest::BaseGpbRequest(VRModule* parent,
                               application_manager::MessagePtr message)
    : parent_(parent),
      json_message_(message) {
}

BaseGpbRequest::~BaseGpbRequest() {
}

void BaseGpbRequest::OnTimeout() {
  LOG4CXX_AUTO_TRACE(logger_);
}

void BaseGpbRequest::Execute() {
  LOG4CXX_AUTO_TRACE(logger_);
}

void BaseGpbRequest::on_event(
    const event_engine::Event<vr_hmi_api::ServiceMessage, vr_hmi_api::RPCName>& event) {
  LOG4CXX_AUTO_TRACE(logger_);
  ProcessEvent(event);  //runs child's logic

}

void BaseGpbRequest::SendNotificationToHMI() {
  LOG4CXX_AUTO_TRACE(logger_);
  json_message_->set_message_type(application_manager::MessageType::kNotification);
  commands::Command* command = CommandFactory::Create(parent_, json_message_);
  if (command) {
    command->Run();
    delete command;
  }
}

void BaseGpbRequest::SendResponseToMobile(bool success, const char* result,
                                          const std::string& info) {
  LOG4CXX_AUTO_TRACE(logger_);
  Json::Value msg_params;
  msg_params[kId] = parent_->service()->GetNextCorrelationID();
  msg_params[kSuccess] = success;
  msg_params[kResultCode] = result;
  msg_params[kInfo] = info;

  json_message_->set_message_type(application_manager::MessageType::kResponse);
  Json::FastWriter writer;
  json_message_->set_json_message(writer.write(msg_params));
  json_message_->set_correlation_id(msg_params[kId].asInt());
  json_message_->set_protocol_version(application_manager::ProtocolVersion::kV2);
  parent_->SendMessageToMobile(json_message_);
}

}  // namespace commands

}  // namespace vr_cooperation
