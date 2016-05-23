/*
 Copyright (c) 2013, Ford Motor Company
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

#ifndef SRC_COMPONENTS_IVDCM_MODULE_INCLUDE_IVDCM_MODULE_EVENT_ENGINE_EVENT_OBSERVER_H_
#define SRC_COMPONENTS_IVDCM_MODULE_INCLUDE_IVDCM_MODULE_EVENT_ENGINE_EVENT_OBSERVER_H_

#include <string>
#include "can_cooperation/event_engine/event.h"


namespace ivdcm_module {

template<typename EventMessage, typename EventID>
class EventObserver {
 public:
  // Typedef for possible Observer ID's from mobile_apis functionID enum
  typedef int64_t ObserverID;

  /*
   * @brief Constructor
   *
   */
  EventObserver();

  /*
   * @brief Destructor
   */
  virtual ~EventObserver();

  /**
   * @brief Retrieves observer unique id
   *
   * @return Unique Observer id
   */
  const ObserverID& id()  const {
    return id_;
  }

  /**
   * @brief Interface method that is called whenever new event received
   *
   * @param event The received event
   */
  virtual void on_event(const Event<EventMessage, EventID>& event) = 0;

 private:
  ObserverID id_;

  DISALLOW_COPY_AND_ASSIGN(EventObserver);
};

template<typename EventMessage, typename EventID>
EventObserver<EventMessage, EventID>::EventObserver()
    : id_(0) {
  // Get unique id based on this
  id_ = reinterpret_cast<ObserverID>(this);
}

template<typename EventMessage, typename EventID>
EventObserver<EventMessage, EventID>::~EventObserver() {
  // unsubscribe_from_all_events();
}

}  // namespace ivdcm_module

#endif  // SRC_COMPONENTS_IVDCM_MODULE_INCLUDE_IVDCM_MODULE_EVENT_ENGINE_EVENT_OBSERVER_H_
