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

#include "vr_cooperation/commands/on_default_service_chosen_notification.h"

#include "functional_module/function_ids.h"
#include "utils/logger.h"
#include "vr_cooperation/vr_module_constants.h"
#include "vr_cooperation/vr_module.h"
#include "vr_cooperation/interface/hmi.pb.h"
#include "vr_cooperation/service_module.h"

namespace vr_cooperation {

namespace commands {

CREATE_LOGGERPTR_GLOBAL(logger_, "VRCooperation")

OnDefaultServiceChosenNotification::OnDefaultServiceChosenNotification(
    ServiceModule* parent, const vr_hmi_api::ServiceMessage& message)
    : JsonNotification(parent, message) {
}

OnDefaultServiceChosenNotification::~OnDefaultServiceChosenNotification() {
}

void OnDefaultServiceChosenNotification::Execute() {
  LOG4CXX_AUTO_TRACE(logger_);

  vr_hmi_api::OnDefaultServiceChosenNotification params;
  vr_hmi_api::ServiceMessage msg = message();
  if (msg.has_params() && params.ParseFromString(msg.params())) {
    if(params.has_appid()) {
      parent()->SetDefaultService(params.appid());
    } else {
      parent()->ResetDefaultService();
    }
  } else {
    LOG4CXX_WARN(logger_, "Could not get result from message");
  }
}

}  // namespace commands

}  // namespace vr_cooperation
