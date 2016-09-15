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
#include "vr_cooperation/commands/support_service_request.h"
#include "vr_cooperation/interface/hmi.pb.h"
#include "vr_cooperation/vr_module_event_gpb.h"

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

class SupportServiceTest : public ::testing::Test {
 protected:
};

TEST_F(SupportServiceTest, Execute) {
  MockServiceModule service;

  const int32_t kId = 1;
  EXPECT_CALL(service, GetNextCorrelationID()).Times(1).WillOnce(Return(kId));
  application_manager::MessagePtr msg = new application_manager::Message(
      protocol_handler::MessagePriority::kDefault);

  SupportServiceRequest cmd(&service, msg);

  vr_hmi_api::ServiceMessage expected;
  expected.set_rpc(vr_hmi_api::SUPPORT_SERVICE);
  expected.set_rpc_type(vr_hmi_api::REQUEST);
  expected.set_correlation_id(kId);
  EXPECT_CALL(service, SendMessageToHMI(ServiceMessageEq(expected))).Times(1);
  cmd.Execute();
}

TEST_F(SupportServiceTest, OnEventSuccess) {
  MockServiceModule service;

  const int32_t kId = 1;
  application_manager::MessagePtr msg = new application_manager::Message(
      protocol_handler::MessagePriority::kDefault);
  SupportServiceRequest cmd(&service, msg);

  vr_hmi_api::ServiceMessage response;
  response.set_rpc(vr_hmi_api::SUPPORT_SERVICE);
  response.set_rpc_type(vr_hmi_api::RESPONSE);
  response.set_correlation_id(kId);
  vr_hmi_api::SupportServiceResponse value;
  value.set_result(vr_hmi_api::SUCCESS);
  std::string value_str;
  value.SerializeToString(&value_str);
  response.set_params(value_str);
  VRModuleEventGPB event(response);

  EXPECT_CALL(service, EnableSupport()).Times(1);
  EXPECT_CALL(service, UnregisterRequest(kId)).Times(1);
  cmd.ProcessEvent(event);
}

TEST_F(SupportServiceTest, OnEventUnsupport) {
  MockServiceModule service;

  const int32_t kId = 1;
  application_manager::MessagePtr msg = new application_manager::Message(
      protocol_handler::MessagePriority::kDefault);
  SupportServiceRequest cmd(&service, msg);

  vr_hmi_api::ServiceMessage response;
  response.set_rpc(vr_hmi_api::SUPPORT_SERVICE);
  response.set_rpc_type(vr_hmi_api::RESPONSE);
  response.set_correlation_id(kId);
  vr_hmi_api::SupportServiceResponse value;
  value.set_result(vr_hmi_api::UNSUPPORTED_RESOURCE);
  std::string value_str;
  value.SerializeToString(&value_str);
  response.set_params(value_str);
  VRModuleEventGPB event(response);

  EXPECT_CALL(service, DisableSupport()).Times(1);
  EXPECT_CALL(service, UnregisterRequest(kId)).Times(1);
  cmd.ProcessEvent(event);
}

TEST_F(SupportServiceTest, OnEventNoParams) {
  MockServiceModule service;

  const int32_t kId = 1;
  application_manager::MessagePtr msg = new application_manager::Message(
      protocol_handler::MessagePriority::kDefault);
  SupportServiceRequest cmd(&service, msg);

  vr_hmi_api::ServiceMessage response;
  response.set_rpc(vr_hmi_api::SUPPORT_SERVICE);
  response.set_rpc_type(vr_hmi_api::RESPONSE);
  response.set_correlation_id(kId);
  VRModuleEventGPB event(response);

  EXPECT_CALL(service, EnableSupport()).Times(0);
  EXPECT_CALL(service, DisableSupport()).Times(0);
  EXPECT_CALL(service, UnregisterRequest(kId)).Times(1);
  cmd.ProcessEvent(event);
}

}  // namespace commands
}  // namespace vr_cooperation
