/*
 * Copyright (c) 2016, Ford Motor Company
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following
 * disclaimer in the documentation and/or other materials provided with the
 * distribution.
 *
 * Neither the name of the Ford Motor Company nor the names of its contributors
 * may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "vr_cooperation/commands/on_service_deactivated_notification.h"

#include "functional_module/function_ids.h"
#include "json/json.h"
#include "utils/logger.h"
#include "vr_cooperation/vr_module_constants.h"

namespace vr_cooperation {

namespace commands {

CREATE_LOGGERPTR_GLOBAL(logger_, "VRCooperation")

OnServiceDeactivatedNotification::OnServiceDeactivatedNotification(
    VRModule* parent, const vr_hmi_api::ServiceMessage& message)
    : parent_(parent),
      message_(message) {
}

void OnServiceDeactivatedNotification::Run() {
  LOG4CXX_AUTO_TRACE(logger_);
  application_manager::MessagePtr mobile_msg = new application_manager::Message(
      protocol_handler::MessagePriority::kDefault);
  mobile_msg->set_function_id(
      functional_modules::MobileFunctionID::ON_SERVICE_DEACTIVATED);
  mobile_msg->set_connection_key(parent_->activated_connection_key());
  mobile_msg->set_message_type(application_manager::MessageType::kNotification);
  mobile_msg->set_correlation_id(message_.correlation_id());
  mobile_msg->set_protocol_version(application_manager::ProtocolVersion::kV3);

  Json::Value msg_params;
  msg_params[json_keys::kService] = kVoiceRecognition;
  Json::FastWriter writer;
  mobile_msg->set_json_message(writer.write(msg_params));
  parent_->service()->SendMessageToMobile(mobile_msg);
  parent_->set_activated_connection_key(-1);
}

void OnServiceDeactivatedNotification::OnTimeout() {
}

}  // namespace commands

}  // namespace vr_cooperation
