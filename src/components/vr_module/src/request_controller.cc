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

#include "vr_module/request_controller.h"
#include "json/json.h"
#include "utils/logger.h"
#include "functional_module/timer/timer_director_impl.h"
#include "functional_module/settings.h"

namespace vr_module {
namespace request_controller {

CREATE_LOGGERPTR_GLOBAL(logger_, "VRModuleRequestController")

RequestController::RequestController() {
  functional_modules::TimeUnit timeout_seconds = 100;
  functional_modules::Settings settings;
  settings.ReadParameter("Remote Control", "timeout_period_seconds",
                         &timeout_seconds);
  timer_.set_period(timeout_seconds);
  LOG4CXX_DEBUG(logger_, "Timeout is set to " << timeout_seconds);
  timer_.AddObserver(this);
  functional_modules::TimerDirector::instance()->RegisterTimer(timer_);
}

RequestController::~RequestController() {
  functional_modules::TimerDirector::instance()->UnregisterTimer(timer_);
  timer_.RemoveObserver(this);
}

void RequestController::AddRequest(const uint32_t& correlation_id,
                                   commands::TimedCommandPtr request) {
  // TODO(VS) Research and fix be problem with overlap correlation ids from two
  // different apllications(on two different mobile devices)
  request_list_[correlation_id] = request;
  // TODO(VS): add app id
  timer_.AddTrackable(TrackableMessage(0, correlation_id));
}

void RequestController::DeleteRequest(const uint32_t& correlation_id) {
  request_list_.erase(correlation_id);
  // TODO(VS): add app id
  timer_.RemoveTrackable(TrackableMessage(0, correlation_id));
}

void RequestController::ResetTimer(const uint32_t& correlation_id) {
  timer_.AddTrackable(TrackableMessage(0, correlation_id));
}

void RequestController::OnTimeoutTriggered(const TrackableMessage& expired) {
  RequestList::iterator it = request_list_.find(expired.correlation_id());
  if (request_list_.end() == it) {
    // no corresponding request found, error.
    return;
  }
  it->second->OnTimeout();
  request_list_.erase(it);
}

}  //  namespace request_controller
}  //  namespace vr_module
