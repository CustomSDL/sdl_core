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

#include "ivdcm_module/commands/on_internet_state_changed_notification.h"
#include "ivdcm_module/ivdcm.h"
#include "ivdcm_module/ivdcm_constants.h"
#include "utils/logger.h"
#include "config_profile/profile.h"
#include "json/json.h"
#include "ivdcm_module/ivdcm_constants.h"
#include "ivdcm_module/interface/rpc.pb.h"
#include "google/protobuf/text_format.h"

#include <string>

namespace ivdcm_module {

namespace commands {

using message_params::kInternetState;

CREATE_LOGGERPTR_GLOBAL(logger_, "OnInternetStateChangedNotification")

OnInternetStateChangedNotification::OnInternetStateChangedNotification(
    Ivdcm* parent)
  : parent_(parent) {
}


void OnInternetStateChangedNotification::Execute(
    const application_manager::MessagePtr& message) {
  LOG4CXX_AUTO_TRACE(logger_);

  sdl_ivdcm_api::OnInternetStateChangedNotificationParams message_params;

  Json::Value value;
  Json::Reader reader;
  reader.parse(message->json_message(), value);

  if (!value.isMember(kInternetState)) {
    LOG4CXX_ERROR(logger_, "Wrong OnInternetStateChangedNotification params!");
    return;
  }

  message_params.set_state(value[kInternetState].asBool());
  message_params.set_nic_name(profile::Profile::instance()->ivdcm_nic_name());
  // TODO(KKolodiy): add virtual network ip
  message_params.set_ip("");

  std::string str;
  message_params.SerializeToString(&str);

  sdl_ivdcm_api::SDLRPC ivdcm_message;

  ivdcm_message.set_params(str);

  ivdcm_message.set_rpc_name(sdl_ivdcm_api::SDLRPC_SDLRPCName::SDLRPC_SDLRPCName_ON_INTERNET_STATE);
  ivdcm_message.set_rpc_type(sdl_ivdcm_api::MessageType::NOTIFICATION);

  parent_->set_connection_key(message->connection_key());
  parent_->SendIvdcmMesssage(ivdcm_message);
}

}  // namespace commands

}  // namespace ivdcm_module
