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

#include "application_manager/message.h"
#include "json/json.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "mock_service_module.h"
#include "vr_cooperation/commands/on_service_deactivated_notification.h"
#include "vr_cooperation/interface/hmi.pb.h"
#include "vr_cooperation/vr_module_constants.h"
#include "vr_cooperation/message_helper.h"

using ::testing::Return;

namespace vr_cooperation {
namespace commands {

MATCHER_P(JsonMessageEq, expected, "") {
  return arg->function_id() == expected->function_id()
      && arg->type() == expected->type()
      && arg->correlation_id() == expected->correlation_id()
      && arg->json_message() == expected->json_message();
}

class OnServiceDeactivatedTest : public ::testing::Test {
 protected:
};

TEST_F(OnServiceDeactivatedTest, Execute) {
  MockServiceModule service;
  const int32_t kMobileId = 1;
  vr_hmi_api::ServiceMessage input;
  input.set_rpc(vr_hmi_api::ON_DEACTIVATED);
  input.set_rpc_type(vr_hmi_api::NOTIFICATION);
  input.set_correlation_id(kMobileId);
  vr_hmi_api::OnServiceDeactivatedNotification notification;
  std::string params;
  notification.SerializeToString(&params);
  input.set_params(params);
  OnServiceDeactivatedNotification cmd(&service, input);

  Json::Value msg_params;
  msg_params[json_keys::kService] = 0;

  application_manager::MessagePtr expected = new application_manager::Message(
      protocol_handler::MessagePriority::kDefault);;
  expected->set_function_id(
      functional_modules::MobileFunctionID::ON_SERVICE_DEACTIVATED);
  expected->set_json_message(MessageHelper::ValueToString(msg_params));
  expected->set_message_type(application_manager::MessageType::kNotification);
  expected->set_protocol_version(application_manager::ProtocolVersion::kV2);
  expected->set_correlation_id(kMobileId);
  expected->set_connection_key(kMobileId);

  EXPECT_CALL(service, activated_connection_key()).Times(1).WillOnce(
          Return(kMobileId));
  EXPECT_CALL(service, SendMessageToMobile(JsonMessageEq(expected))).Times(1);
  EXPECT_CALL(service, DeactivateService()).Times(1);
  cmd.Execute();
}

}  // namespace commands
}  // namespace vr_cooperation
