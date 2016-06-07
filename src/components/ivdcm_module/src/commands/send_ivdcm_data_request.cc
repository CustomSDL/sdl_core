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

#include "ivdcm_module/commands/send_ivdcm_data_request.h"
#include "ivdcm_module/event_engine/event_dispatcher.h"
#include "ivdcm_module/interface/rpc.pb.h"
#include "ivdcm_module/ivdcm_constants.h"
#include "google/protobuf/text_format.h"
#include "utils/logger.h"
#include "json/json.h"

#include <vector>

namespace ivdcm_module {

namespace commands {

using event_engine::EventDispatcher;

using message_params::kUrl;
using message_params::kSuccess;
using message_params::kResultCode;
using message_params::kInfo;

CREATE_LOGGERPTR_GLOBAL(logger_, "SendIvdcmDataRequest")

SendIvdcmDataRequest::SendIvdcmDataRequest(Ivdcm*  parent)
  : parent_(parent) {
}


SendIvdcmDataRequest::~SendIvdcmDataRequest() {
  EventDispatcher<application_manager::MessagePtr,
                  functional_modules::MobileFunctionID>::instance()->
                    remove_observer(this);
}


void SendIvdcmDataRequest::OnTimeout() {
  LOG4CXX_AUTO_TRACE(logger_);
  // TODO(VS): add timeout processing
}

// TODO(VS): Add to SendRequest and on_event error handling, messages validation and error responses
void  SendIvdcmDataRequest::SendRequest(const sdl_ivdcm_api::SDLRPC& message) {
  LOG4CXX_AUTO_TRACE(logger_);

  // First parse message params
  sdl_ivdcm_api::SendIvdcmDataRequestParams message_params;

  if (!message_params.ParseFromString(message.params())) {
    LOG4CXX_ERROR(logger_, "Wrong SendIvdcmDataRequest params!");
    return;
  }

  // If params ok, create request message for mobile
  application_manager::MessagePtr mobile_msg = new application_manager::Message(
      protocol_handler::MessagePriority::kDefault);

  mobile_msg->set_function_id(
      functional_modules::MobileFunctionID::SEND_IVDCM_DATA);
  mobile_msg->set_connection_key(parent_->connection_key());
  mobile_msg->set_correlation_id(Ivdcm::GetNextCorrelationID());
  mobile_msg->set_message_type(application_manager::MessageType::kRequest);

  mobile_msg->set_protocol_version(application_manager::ProtocolVersion::kV2);

  // Add message to event dispatcher
  EventDispatcher<application_manager::MessagePtr,
                  functional_modules::MobileFunctionID>::instance()->
    add_observer(functional_modules::MobileFunctionID::SEND_IVDCM_DATA,
                 mobile_msg->correlation_id(), this);

  // Add params to mobile request
  Json::Value value;

  value[kUrl] = message_params.url();

  Json::FastWriter writer;
  mobile_msg->set_json_message(writer.write(value));

  // Post non ip request
  if (message_params.has_upload_data()) {
    binary_data_.assign(message_params.upload_data().begin(),
                       message_params.upload_data().end());
    mobile_msg->set_binary_data(&binary_data_);
    mobile_msg->set_data_size(binary_data_.size());
  }

  parent_->AddRequestToRequestController(mobile_msg->correlation_id(), this);
  parent_->SendMessageToMobile(mobile_msg);
}

void SendIvdcmDataRequest::on_event(
    const event_engine::Event<application_manager::MessagePtr,
                              functional_modules::MobileFunctionID>& event) {
  LOG4CXX_AUTO_TRACE(logger_);

  sdl_ivdcm_api::SendIvdcmDataResponseParams message_params;

  Json::Value value;
  Json::Reader reader;
  reader.parse(event.event_message()->json_message(), value);

  message_params.set_result_code(static_cast<sdl_ivdcm_api::ResultCode>(
      value[kResultCode].asUInt()));
  // value[kSuccess].asBool();

  message_params.set_response_data(
                      std::string(event.event_message()->binary_data()->begin(),
                                  event.event_message()->binary_data()->end()));

  if (value.isMember(kInfo)) {
    message_params.set_info(value[kInfo].asString());
  }

  std::string str;
  google::protobuf::TextFormat::PrintToString(message_params, &str);

  sdl_ivdcm_api::SDLRPC message;

  message.set_params(str);

  message.set_rpc_name(sdl_ivdcm_api::SDLRPCName::SEND_IVDCM_DATA);
  message.set_rpc_type(sdl_ivdcm_api::MessageType::RESPONSE);

  parent_->SendIvdcmMesssage(message);

  if (sdl_ivdcm_api::ResultCode::PARTIAL_CONTENT ==
      message_params.result_code()) {
    parent_->ResetTimer(event.event_message_correlation_id());
  } else {
    parent_->DeleteRequestFromRequestController(
        event.event_message_correlation_id());
  }
}


}  // namespace commands
}  // namespace ivdcm_module

