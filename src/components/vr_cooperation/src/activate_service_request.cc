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
#include "utils/logger.h"

namespace vr_cooperation {

namespace commands {

CREATE_LOGGERPTR_GLOBAL(logger_, "ActivateServiceRequest")

ActivateServiceRequest::ActivateServiceRequest(
      VRModule* parent,
      const application_manager::MessagePtr& message)
  : BaseCommandRequest(parent, message) {
}

ActivateServiceRequest::~ActivateServiceRequest() {
}

void ActivateServiceRequest::Execute() {
  LOG4CXX_AUTO_TRACE(logger_);
  Json::Value params;
  Json::Reader reader;
  reader.parse(message_->json_message(), params);

  SendRequest(
      functional_modules::hmi_api::activate_service,
      params, true);
}

void ActivateServiceRequest::OnEvent(const event_engine::Event<application_manager::MessagePtr,
    std::string>& event) {
  LOG4CXX_AUTO_TRACE(logger_);
  Json::Value value;
  Json::Reader reader;
  reader.parse(event.event_message()->json_message(), value);

  int result_code;
  std::string info;
  bool success = ParseMobileResultCode(value, result_code, info);

  SendResponse(success, result_code, info, true);
}

}  // namespace commands

}  // namespace vr_cooperation
