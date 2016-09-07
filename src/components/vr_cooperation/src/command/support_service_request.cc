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

#include "vr_cooperation/commands/support_service_request.h"
#include "vr_cooperation/vr_module.h"
#include "utils/logger.h"

namespace vr_cooperation {

namespace commands {

CREATE_LOGGERPTR_GLOBAL(logger_, "SupportServiceRequest")

SupportServiceRequest::SupportServiceRequest(
    VRModule* parent, application_manager::MessagePtr message)
    : BaseGpbRequest(parent, message),
      message_() {
}

SupportServiceRequest::~SupportServiceRequest() {
}

void SupportServiceRequest::Execute() {
  LOG4CXX_AUTO_TRACE(logger_);
  message_.set_rpc(vr_hmi_api::SUPPORT_SERVICE);
  message_.set_rpc_type(vr_hmi_api::REQUEST);
  message_.set_correlation_id(parent()->GetNextCorrelationID());
  parent()->SendMessageToHMI(message_);
}

void SupportServiceRequest::OnEvent(
    const event_engine::Event<vr_hmi_api::ServiceMessage, vr_hmi_api::RPCName>& event) {
  LOG4CXX_AUTO_TRACE(logger_);
  const vr_hmi_api::ServiceMessage message = event.event_message();
  if (!message.has_params()) {
    LOG4CXX_WARN(logger_, "Message does not contain params");
    return;
  }
  vr_hmi_api::SupportServiceResponse response;
  if (response.ParseFromString(message.params())) {
    const bool supported = vr_hmi_api::SUCCESS == response.result();
    parent()->set_supported(supported);
    parent()->UnregisterRequest(message.correlation_id());
  } else {
    LOG4CXX_WARN(logger_, "Could not parse params");
  }
}

}  // namespace commands

}  // namespace vr_cooperation
