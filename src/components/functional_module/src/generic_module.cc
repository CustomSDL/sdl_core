/*
 * Copyright (c) 2013, Ford Motor Company
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

#include "functional_module/generic_module.h"

namespace functional_modules {

typedef std::deque<ModuleObserver*>::iterator ModuleObserverIterator;

GenericModule::GenericModule(ModuleID module_id)
  : kModuleId_(module_id)
  , state_(ServiceState::IDLE) {}

GenericModule::~GenericModule() {
  observers_.clear();
}

void GenericModule::AddObserver(ModuleObserver* const observer) {
  DCHECK(observer);
  if (!observer) {
    return;
  }
  observers_.push_back(observer);
}

void GenericModule::RemoveObserver(ModuleObserver* const observer) {
  DCHECK(observer);
  if (!observer) {
    return;
  }
  for (ModuleObserverIterator it = observers_.begin();
       observers_.end() != it;
       ++it) {
    if (*it == observer) {
      observers_.erase(it);
      return;
    }
  }
}

void GenericModule::NotifyObservers(ModuleObserver::Errors error) {
  for (ModuleObserverIterator it = observers_.begin();
       observers_.end() != it; ++it) {
    (*it)->OnError(error, kModuleId_);
  }
}

void GenericModule::set_service(application_manager::ServicePtr service) {
  service_ = service;
}

void GenericModule::OnServiceStateChanged(ServiceState state) {
  state_ = state;

  if (HMI_ADAPTER_INITIALIZED == state_) {
    // We must subscribe to necessary HMI notifications
    service_->SubscribeToHMINotification(hmi_api::on_interior_vehicle_data);
    service_->SubscribeToHMINotification(hmi_api::on_reverse_apps_allowing);
    service_->SubscribeToHMINotification(hmi_api::on_device_rank_changed);
    service_->SubscribeToHMINotification(hmi_api::on_device_location_changed);
  }
}

application_manager::ServicePtr GenericModule::service() {
  return service_;
}

}  //  namespace functional_modules
