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

#include "vr_cooperation/vr_module.h"

#include "functional_module/function_ids.h"
#include "google/protobuf/text_format.h"
#include "json/json.h"
#include "vr_cooperation/commands/on_service_deactivated_notification.h"
#include "vr_cooperation/event_engine/event_dispatcher.h"
#include "vr_cooperation/interface/hmi.pb.h"
#include "vr_cooperation/mobile_command_factory.h"
#include "vr_cooperation/message_helper.h"
#include "vr_cooperation/vr_module_event.h"
#include "vr_cooperation/vr_module_constants.h"
#include "utils/logger.h"

namespace vr_cooperation {

using functional_modules::ProcessResult;
using functional_modules::GenericModule;
using functional_modules::PluginInfo;
using functional_modules::MobileFunctionID;
using event_engine::EventDispatcher;
namespace hmi_api = functional_modules::hmi_api;

using json_keys::kId;
using json_keys::kMethod;

PLUGIN_FACTORY(VRModule)

CREATE_LOGGERPTR_GLOBAL(logger_, "VRCooperation")

VRModule::VRModule()
    : GenericModule(kVRModuleID),
      proxy_(this),
      activated_connection_key_(-1),
      default_app_id_(-1) {
  plugin_info_.name = "VRModule";
  plugin_info_.version = 1;
  SubcribeToRPCMessage();
}

VRModule::~VRModule() {
}

void VRModule::SubcribeToRPCMessage() {
  plugin_info_.mobile_function_list.push_back(
      MobileFunctionID::REGISTER_SERVICE);

  plugin_info_.mobile_function_list.push_back(
      MobileFunctionID::UNREGISTER_SERVICE);

  plugin_info_.mobile_function_list.push_back(
      MobileFunctionID::ACTIVATE_SERVICE);

  plugin_info_.mobile_function_list.push_back(
      MobileFunctionID::ON_SERVICE_DEACTIVATED);

  plugin_info_.mobile_function_list.push_back(
      MobileFunctionID::PROCESS_DATA);

  plugin_info_.hmi_function_list.push_back(hmi_api::activate_service);
  plugin_info_.hmi_function_list.push_back(hmi_api::on_register_service);
  plugin_info_.hmi_function_list.push_back(hmi_api::on_unregister_service);
  plugin_info_.hmi_function_list.push_back(hmi_api::on_service_deactivated);
  plugin_info_.hmi_function_list.push_back(hmi_api::on_default_service_chosen);
}

functional_modules::PluginInfo VRModule::GetPluginInfo() const {
  LOG4CXX_AUTO_TRACE(logger_);
  return plugin_info_;
}

functional_modules::ProcessResult VRModule::ProcessMessage(
    application_manager::MessagePtr msg) {
  LOG4CXX_AUTO_TRACE(logger_);
  DCHECK(msg);

  if (!msg) {
    LOG4CXX_ERROR(logger_, "Null pointer message received.");
    return ProcessResult::FAILED;
  }

  msg->set_function_name(MessageHelper::GetMobileAPIName(
      static_cast<functional_modules::MobileFunctionID>(msg->function_id())));

  LOG4CXX_DEBUG(logger_, "Mobile message: " << msg->json_message());
  return HandleMessage(msg);
}

functional_modules::ProcessResult VRModule::HandleMessage(
  application_manager::MessagePtr msg) {
  LOG4CXX_AUTO_TRACE(logger_);

  if (!SetMessageType(msg)) {
    return ProcessResult::FAILED;
  }
  msg->set_protocol_version(application_manager::ProtocolVersion::kV3);

  switch (msg->type()) {
    case application_manager::MessageType::kResponse:
    case application_manager::MessageType::kErrorResponse: {
      if (functional_modules::hmi_api::activate_service
          == msg->function_name()) {
        VRModuleEvent event(msg, MobileFunctionID::ACTIVATE_SERVICE);
        EventDispatcher<application_manager::MessagePtr,
          functional_modules::MobileFunctionID>::instance()->raise_event(event);
      }
      break;
    }
    case application_manager::MessageType::kRequest: {
      commands::Command* command = MobileCommandFactory::CreateCommand(this, msg);
      if (command) {
        request_controller_.AddRequest(msg->correlation_id(), command);
        command->Run();
      } else {
        return ProcessResult::CANNOT_PROCESS;
      }
      break;
    }
    default: {
      return ProcessResult::FAILED;
    }
  }
  return ProcessResult::PROCESSED;
}

functional_modules::ProcessResult VRModule::ProcessHMIMessage(
    application_manager::MessagePtr msg) {
  LOG4CXX_AUTO_TRACE(logger_);
  // Json message between plugin and hmi will not handle anymore
  return ProcessResult::NONE;
}

bool VRModule::SetMessageType(application_manager::MessagePtr msg) const {
  LOG4CXX_AUTO_TRACE(logger_);
  Json::Value value;
  Json::Reader reader;
  reader.parse(msg->json_message(), value);
  bool result = true;

  if (value.isMember(json_keys::kMethod)) {
    value.isMember(json_keys::kId) ?
        msg->set_message_type(application_manager::MessageType::kRequest) :
        msg->set_message_type(application_manager::MessageType::kNotification);
  } else if (value.isMember(json_keys::kResult)
             && value[json_keys::kResult].isMember(json_keys::kMethod)) {
    msg->set_message_type(application_manager::MessageType::kResponse);
  } else if (value.isMember(json_keys::kError)
             && value[json_keys::kError].isMember(json_keys::kData)
             && value[json_keys::kError][json_keys::kData].isMember(
                 json_keys::kMethod)) {
    msg->set_message_type(application_manager::MessageType::kErrorResponse);
  } else {
    DCHECK(false);
    result = false;
  }

  if (value.isMember(json_keys::kId)) {
    msg->set_correlation_id(value[json_keys::kId].asInt());
  } else if (application_manager::MessageType::kNotification != msg->type()) {
    DCHECK(false);
    result = false;
  }
  return result;
}

void VRModule::RemoveAppExtension(uint32_t app_id) {
  LOG4CXX_AUTO_TRACE(logger_);
  // This function is not implemented since there is no app extension
  // functionality in VR Service
}

void VRModule::RemoveAppExtensions() {
  LOG4CXX_AUTO_TRACE(logger_);
  // This function is not implemented since there is no app extension
  // functionality in VR Service
}

bool VRModule::IsAppForPlugin(application_manager::ApplicationSharedPtr app) {
  LOG4CXX_AUTO_TRACE(logger_);
  application_manager::AppExtensionPtr app_extension = app->QueryInterface(
      GetModuleID());
  if (app_extension) {
    return true;
  }
  // This function is not implemented since there is no app extension
  // functionality in VR Service
  return false;
}

void VRModule::OnAppHMILevelChanged(
    application_manager::ApplicationSharedPtr app,
    mobile_apis::HMILevel::eType) {
  LOG4CXX_AUTO_TRACE(logger_);
}

void VRModule::OnDeviceRemoved(const connection_handler::DeviceHandle& device) {
  LOG4CXX_AUTO_TRACE(logger_);
}

void VRModule::SendMessageToHMI(application_manager::MessagePtr msg) {
  LOG4CXX_DEBUG(logger_, "Message to HMI: " << msg->json_message());
  service()->SendMessageToHMI(msg);
  request_controller_.DeleteRequest(msg->correlation_id());
}

void VRModule::SendMessageToHMI(const vr_hmi_api::ServiceMessage& message) {
  LOG4CXX_AUTO_TRACE(logger_);
  std::string str;
  google::protobuf::TextFormat::PrintToString(message, &str);
  LOG4CXX_DEBUG(logger_, "Protobuf message to HMI: " << str);
  proxy_.Send(message);
}

void VRModule::SendMessageToMobile(application_manager::MessagePtr msg) {
  LOG4CXX_DEBUG(logger_, "Message to mobile: " << msg->json_message());
  service()->SendMessageToMobile(msg);
  request_controller_.DeleteRequest(msg->correlation_id());
}

void VRModule::OnReceived(const vr_hmi_api::ServiceMessage& message) {
  LOG4CXX_AUTO_TRACE(logger_);
  std::string str;
  google::protobuf::TextFormat::PrintToString(message, &str);
  LOG4CXX_DEBUG(logger_, "Protobuf message: " << str);

  if (vr_hmi_api::MessageType::RESPONSE == message.rpc_type()) {
    if (vr_hmi_api::RPCName::SUPPORT_SERVICE == message.rpc()) {
      // TODO(Giang): raise_event after VRModuleEvent for GPB implementation
    }
  } else {
//    Should un-comment when HMICommandFactory implementation
//    commands::Command* command = HMICommandFactory::CreateCommand(this, message);
//    if (command) {
//      if (vr_hmi_api::MessageType::REQUEST == message.rpc_type()) {
//        request_controller_.AddRequest(message.correlation_id(), command);
//      }
//      command->Run();
//    }
  }
}

}  // namespace vr_cooperation

