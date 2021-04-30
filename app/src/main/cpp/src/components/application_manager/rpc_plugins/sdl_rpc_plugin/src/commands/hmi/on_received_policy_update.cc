/*
 * Copyright (c) 2018, Ford Motor Company
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

#include "sdl_rpc_plugin/commands/hmi/on_received_policy_update.h"
#include <string>
#include "application_manager/application_manager.h"
#include "application_manager/policies/policy_handler_interface.h"
#include "utils/file_system.h"

namespace sdl_rpc_plugin {
using namespace application_manager;

namespace commands {

SDL_CREATE_LOG_VARIABLE("Commands")

OnReceivedPolicyUpdate::OnReceivedPolicyUpdate(
    const application_manager::commands::MessageSharedPtr& message,
    ApplicationManager& application_manager,
    rpc_service::RPCService& rpc_service,
    HMICapabilities& hmi_capabilities,
    policy::PolicyHandlerInterface& policy_handle)
    : NotificationFromHMI(message,
                          application_manager,
                          rpc_service,
                          hmi_capabilities,
                          policy_handle) {}

OnReceivedPolicyUpdate::~OnReceivedPolicyUpdate() {}

void OnReceivedPolicyUpdate::Run() {
  SDL_LOG_AUTO_TRACE();
#if defined(PROPRIETARY_MODE) || defined(EXTERNAL_PROPRIETARY_MODE)
  const std::string& file_path =
      (*message_)[strings::msg_params][hmi_notification::policyfile].asString();
  policy::BinaryMessage file_content;

  std::string file_name = file_path;

#ifdef __ANDROID__
  const std::string t_name = file_path.substr(file_path.find_last_of("/\\") + 1);
  file_name = "/data/user/0/org.luxoft.sdl_core/cache/ivsu_cache/" + t_name;
#endif

  if (!file_system::ReadBinaryFile(file_name, file_content)) {
    SDL_LOG_ERROR("Failed to read Update file.");
    return;
  }
  policy_handler_.ReceiveMessageFromSDK(file_name, file_content);
#else
  SDL_LOG_WARN(
      "This RPC is part of extended policy flow. "
      "Please re-build with extended policy mode enabled.");
#endif
}

}  // namespace commands

}  // namespace sdl_rpc_plugin
