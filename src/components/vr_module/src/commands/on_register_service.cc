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

#include "vr_module/commands/on_register_service.h"

#include "utils/logger.h"
#include "vr_module/service_module.h"

namespace vr_module {
namespace commands {

CREATE_LOGGERPTR_GLOBAL(logger_, "VRModule")

OnRegisterService::OnRegisterService(const vr_hmi_api::ServiceMessage& message,
                                     ServiceModule* module)
    : module_(module),
      message_(message) {
  message_.set_rpc_type(vr_hmi_api::NOTIFICATION);
  message_.set_correlation_id(module_->GetNextCorrelationID());
}

bool OnRegisterService::Execute() {
  LOG4CXX_AUTO_TRACE(logger_);
  vr_hmi_api::OnRegisterServiceNotification notification;
  if (notification.ParseFromString(message_.params())) {
    notification.set_default_(module_->IsDefaultService(notification.appid()));
    std::string params;
    if (notification.SerializeToString(&params)) {
      message_.set_params(params);
      return module_->SendToHmi(message_);
    } else {
      LOG4CXX_ERROR(logger_, "Could not serialize message");
    }
  } else {
    LOG4CXX_ERROR(logger_, "Could not get application id from message");
  }
  return false;
}

}  // namespace commands
}  // namespace vr_module
