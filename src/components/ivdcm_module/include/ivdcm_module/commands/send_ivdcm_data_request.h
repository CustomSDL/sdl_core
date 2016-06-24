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

#ifndef SRC_COMPONENTS_IVDCM_MODULE_INCLUDE_IVDCM_MODULE_COMMANDS_SEND_IVDCM_DATA_REQUEST_H_
#define SRC_COMPONENTS_IVDCM_MODULE_INCLUDE_IVDCM_MODULE_COMMANDS_SEND_IVDCM_DATA_REQUEST_H_

#include <string>

#include "ivdcm_module/ivdcm.h"
#include "ivdcm_module/commands/command.h"
#include "ivdcm_module/event_engine/event_observer.h"
#include "application_manager/message.h"

namespace ivdcm_module {

namespace commands {

/**
 * @brief SendIvdcmData class for request to mobile
 */
class SendIvdcmDataRequest : public Command,
  public event_engine::EventObserver<application_manager::MessagePtr,
                                     functional_modules::MobileFunctionID> {
 public:
  explicit SendIvdcmDataRequest(Ivdcm*  parent);

  /**
   * @brief SendIvdcmDataRequest class destructor
   */
  virtual ~SendIvdcmDataRequest();

  /**
   * @brief Sends request to mobile
   */
  void SendRequest(const sdl_ivdcm_api::SDLRPC& message);

  void SendRequest(application_manager::BinaryData* binary_data);

  /**
   * @brief SendIvdcmDataRequest on timeout reaction
   */
  virtual void OnTimeout();

  void on_event(const event_engine::Event<application_manager::MessagePtr,
                functional_modules::MobileFunctionID>& event);

 private:

  void SendRequest(const Json::Value params,
                   application_manager::BinaryData* binary_data);

  Ivdcm* parent_;

  std::vector<uint8_t> binary_data_;

  DISALLOW_COPY_AND_ASSIGN(SendIvdcmDataRequest);
};

}  // namespace commands

}  // namespace ivdcm_module

#endif  // SRC_COMPONENTS_IVDCM_MODULE_INCLUDE_IVDCM_MODULE_COMMANDS_SEND_IVDCM_DATA_REQUEST_H_
