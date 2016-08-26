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

namespace vr_cooperation {

namespace commands {

CREATE_LOGGERPTR_GLOBAL(logger_, "VRCooperation")

OnDefaultServiceChosenNotification::OnDefaultServiceChosenNotification(
    VRModule* parent, const vr_hmi_api::ServiceMessage& message)
    : message_(message),
      parent_(parent) {
}

OnDefaultServiceChosenNotification::~OnDefaultServiceChosenNotification() {
}

void OnDefaultServiceChosenNotification::Run() {
  LOG4CXX_AUTO_TRACE(logger_);

  int32_t app_id = -1;
  vr_hmi_api::OnDefaultServiceChosenNotification params;
  if (message_.has_params()) {
    bool ret = params.ParseFromString(message_.params());
    if (ret) {
      app_id = params.has_appid() ? params.appid() : -1;
      parent_->set_default_app_id(app_id);
    } else {
      LOG4CXX_WARN(logger_, "Could not parse params");
    }
  }
}

void OnDefaultServiceChosenNotification::OnTimeout() {
  LOG4CXX_AUTO_TRACE(logger_);
}

}  // namespace commands

}  // namespace vr_cooperation
