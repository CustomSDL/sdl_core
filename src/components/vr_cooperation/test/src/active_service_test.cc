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

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "mock_service_module.h"
#include "vr_cooperation/commands/activate_service_request.h"
#include "vr_cooperation/interface/hmi.pb.h"
//#include "vr_cooperation/mobile_event.h"

using ::testing::_;
using ::testing::Return;

namespace vr_cooperation {
namespace commands {

MATCHER_P(ServiceMessageEq, expected, "") {
  return arg.rpc() == expected.rpc()
      && arg.rpc_type() == expected.rpc_type()
      && arg.correlation_id() == expected.correlation_id()
      && arg.params() == expected.params();
}

MATCHER_P(JsonMessageEq, expected, "") {
  return arg->function_id() == expected->function_id()
      && arg->type() == expected->type()
      && arg->correlation_id() == expected->correlation_id()
      && arg->json_message() == expected->json_message();
}

class ActivateServiceTest : public ::testing::Test {
 protected:
};

TEST_F(ActivateServiceTest, Execute) {
  MockServiceModule service;

  const int32_t kHmiId = 1;
  const int32_t kMobileId = 2;
  const int32_t kAppId = 3;
  EXPECT_CALL(service, GetNextCorrelationID()).Times(1).WillOnce(
      Return(kMobileId));
  vr_hmi_api::ServiceMessage input;
  input.set_rpc(vr_hmi_api::ACTIVATE);
  input.set_rpc_type(vr_hmi_api::REQUEST);
  input.set_correlation_id(kHmiId);
  vr_hmi_api::ActivateServiceRequest request;
  request.set_appid(kAppId);
  std::string params;
  request.SerializeToString(&params);
  input.set_params(params);
  ActivateServiceRequest cmd(&service, input);

  //Prepare expected json message:
  application_manager::MessagePtr expected = new application_manager::Message(
      protocol_handler::MessagePriority::kDefault);;
  expected->set_function_id(
      functional_modules::MobileFunctionID::ACTIVATE_SERVICE);
  expected->set_json_message(MessageHelper::ValueToString(msg_params));
  expected->set_message_type(application_manager::MessageType::kRequest);
  expected->set_protocol_version(application_manager::ProtocolVersion::kV2);
  expected->set_correlation_id(kMobileId);
//
  EXPECT_CALL(service, SendMessageToMobile(JsonMessageEq(expected))).Times(1);

//  EXPECT_CALL(service, ActivateService(kAppId)).Times(1);
  cmd.Execute();
}

TEST_F(ActivateServiceTest, ExecuteNoParams) {
  MockServiceModule service;

  const int32_t kHmiId = 1;
  const int32_t kMobileId = 2;
  EXPECT_CALL(service, GetNextCorrelationID()).Times(1).WillOnce(
      Return(kMobileId));
  vr_hmi_api::ServiceMessage input;
  input.set_rpc(vr_hmi_api::ACTIVATE);
  input.set_rpc_type(vr_hmi_api::REQUEST);
  input.set_correlation_id(kHmiId);
  ActivateServiceRequest cmd(&service, input);

//  vr_hmi_api::ServiceMessage expected;
//  expected.set_rpc(vr_hmi_api::ACTIVATE);
//  expected.set_rpc_type(vr_hmi_api::RESPONSE);
//  expected.set_correlation_id(kHmiId);
//  vr_hmi_api::ActivateServiceResponse hmi_response;
//  hmi_response.set_result(vr_hmi_api::INVALID_DATA);
//  std::string hmi_params;
//  hmi_response.SerializeToString(&hmi_params);
//  expected.set_params(hmi_params);
//  EXPECT_CALL(service, SendMessageToHmi(ServiceMessageEq(expected))).Times(1)
//      .WillOnce(Return(true));
//  EXPECT_CALL(service, SendMessageToMobile(_)).Times(0);
  EXPECT_CALL(service, ActivateService(_)).Times(0);
  EXPECT_CALL(service, UnregisterRequest(kMobileId)).Times(1);
  cmd.Execute();
}

}  // namespace commands
}  // namespace vr_cooperation
