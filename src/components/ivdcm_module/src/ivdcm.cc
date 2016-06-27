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

#include "ivdcm_module/ivdcm.h"

#include "ivdcm_module/commands/send_ivdcm_data_request.h"
#include "ivdcm_module/commands/on_internet_state_changed_notification.h"
#include "ivdcm_module/ivdcm_event.h"
#include "ivdcm_module/ivdcm_proxy.h"
#include "ivdcm_module/event_engine/event_dispatcher.h"
#include "config_profile/profile.h"
#include "google/protobuf/text_format.h"
#include "interface/rpc.pb.h"
#include "utils/logger.h"

namespace ivdcm_module {

using event_engine::EventDispatcher;

using functional_modules::ProcessResult;
using functional_modules::GenericModule;
using functional_modules::PluginInfo;
using functional_modules::MobileFunctionID;

CREATE_LOGGERPTR_GLOBAL(logger_, "IVDCM")

PLUGIN_FACTORY(Ivdcm)

uint32_t Ivdcm::next_correlation_id_ = 1;

Ivdcm::Ivdcm()
    : GenericModule(kModuleID),
      proxy_(0),
      tun_id_(-1),
      connection_key_(0) {
  plugin_info_.name = "IvdcmPlugin";
  plugin_info_.version = 1;
  SubscribeToRpcMessages();
  // TODO(KKolodiy) workaround for reading profile,
  // possible it is fixed in fresh version of OpenSDL
  profile::Profile::instance()->config_file_name("smartDeviceLink.ini");
  proxy_ = new (std::nothrow) IvdcmProxy(this);
}

Ivdcm::~Ivdcm() {
  delete proxy_;
}

void Ivdcm::OnInternetStateChanged(bool state, std::string* nic,
                                   std::string* ip) {
  LOG4CXX_AUTO_TRACE(logger_);
  if (state) {
    tun_id_ = proxy_->CreateTun();
    *nic = proxy_->GetNameTun(tun_id_);
    *ip = proxy_->GetAddressTun(tun_id_);
  } else {
    proxy_->DestroyTun(tun_id_);
    tun_id_ = -1;
  }
}

void Ivdcm::OnReceived(int id, const std::vector<uint8_t>& packet) {
  LOG4CXX_AUTO_TRACE(logger_);
  std::vector<uint8_t> *data = new std::vector<uint8_t>(packet.begin(),
                                                        packet.end());
  // TODO(KKolodiy): should be implemented
  UNUSED(data);
}

void Ivdcm::SendIvdcmIpPacket(const std::vector<uint8_t>& packet) {
  LOG4CXX_AUTO_TRACE(logger_);
  proxy_->Send(tun_id_, packet);
}

void Ivdcm::SubscribeToRpcMessages() {
  plugin_info_.mobile_function_list.push_back(
      MobileFunctionID::ON_INTERNET_STATE_CHANGED);
  plugin_info_.mobile_function_list.push_back(
      MobileFunctionID::SEND_IVDCM_DATA);
}

PluginInfo Ivdcm::GetPluginInfo() const {
  LOG4CXX_AUTO_TRACE(logger_);
  return plugin_info_;
}

ProcessResult Ivdcm::ProcessHMIMessage(application_manager::MessagePtr msg) {
  LOG4CXX_AUTO_TRACE(logger_);
  DCHECK(msg);
  LOG4CXX_DEBUG(logger_, "HMI message: " << msg->json_message());
  if (!msg) {
    LOG4CXX_ERROR(logger_, "Null pointer message received.");
    return ProcessResult::FAILED;
  }

  // TODO(KKolodiy): here should be implemented logic of the message processing

  return ProcessResult::PROCESSED;
}

void Ivdcm::RemoveAppExtension(uint32_t app_id) {
  LOG4CXX_AUTO_TRACE(logger_);
  application_manager::ApplicationSharedPtr app = service()->GetApplication(
        app_id);

  if (app) {
    app->RemoveExtension(GetModuleID());
  }
}

void Ivdcm::OnDeviceRemoved(const connection_handler::DeviceHandle& device) {
  LOG4CXX_AUTO_TRACE(logger_);
  // TODO(KKolodiy): here should be implemented logic of the device removing
}

void Ivdcm::RemoveAppExtensions() {
  LOG4CXX_AUTO_TRACE(logger_);
  std::vector<application_manager::ApplicationSharedPtr> applications =
    service()->GetApplications(GetModuleID());

  std::vector<application_manager::ApplicationSharedPtr>::iterator it =
    applications.begin();

  for (; it != applications.end(); ++it) {
    application_manager::ApplicationSharedPtr app = *it;
    if (app) {
      app->RemoveExtension(GetModuleID());
    }
  }
}

bool Ivdcm::IsAppForPlugin(application_manager::ApplicationSharedPtr app) {
  LOG4CXX_AUTO_TRACE(logger_);
  application_manager::AppExtensionPtr app_extension = app->QueryInterface(
    GetModuleID());
  if (app_extension) {
    return true;
  }

  // TODO(KKolodiy): here should be implemented logic of the checking

  return false;
}

void Ivdcm::OnAppHMILevelChanged(application_manager::ApplicationSharedPtr app,
    mobile_apis::HMILevel::eType) {
  LOG4CXX_AUTO_TRACE(logger_);
  LOG4CXX_DEBUG(logger_, "Application " << app->name()
        << " has changed hmi level to " << app->hmi_level());
  // TODO(KKolodiy): here should be implemented the corresponding logic
}

void Ivdcm::SendMessageToMobile(application_manager::MessagePtr msg) {
  LOG4CXX_DEBUG(logger_, "Message to mobile: " << msg->json_message());
  service()->SendMessageToMobile(msg);
}

ProcessResult Ivdcm::ProcessMessage(application_manager::MessagePtr msg) {
  DCHECK(msg);

  if (!msg) {
    LOG4CXX_ERROR(logger_, "Null pointer message received.");
    return ProcessResult::FAILED;
  }

  LOG4CXX_DEBUG(logger_, "Mobile message: " << msg->json_message());

  switch (msg->function_id()) {
    case MobileFunctionID::ON_INTERNET_STATE_CHANGED: {
      commands::OnInternetStateChangedNotification notification(this);
      notification.Execute(msg);
      return ProcessResult::PROCESSED;
    }
    case MobileFunctionID::SEND_IVDCM_DATA: {
      IvdcmEvent event(msg, MobileFunctionID::SEND_IVDCM_DATA);
      EventDispatcher<application_manager::MessagePtr,
                      functional_modules::MobileFunctionID>::instance()->
                      raise_event(event);
      return ProcessResult::PROCESSED;
    }
    default: {
      return ProcessResult::CANNOT_PROCESS;
    }
  }
}

void Ivdcm::OnReceived(const sdl_ivdcm_api::SDLRPC &message) {
  LOG4CXX_AUTO_TRACE(logger_);
  std::string str;
  google::protobuf::TextFormat::PrintToString(message, &str);
  LOG4CXX_DEBUG(logger_, "Protobuf message: " << str);

  commands::SendIvdcmDataRequest* request = new commands::SendIvdcmDataRequest(
                                                                          this);

  if (request) {
    request->SendRequest(message);
  }
}

void  Ivdcm::SendIvdcmMesssage(const sdl_ivdcm_api::SDLRPC &message) {
  LOG4CXX_AUTO_TRACE(logger_);
  proxy_->Send(message);
}

void Ivdcm::AddRequestToRequestController(uint32_t mobile_correlation_id,
                                 request_controller::MobileRequestPtr request) {
  LOG4CXX_AUTO_TRACE(logger_);
  request_controller_.AddRequest(mobile_correlation_id, request);
}

void Ivdcm::DeleteRequestFromRequestController(uint32_t mobile_correlation_id) {
  LOG4CXX_AUTO_TRACE(logger_);
  request_controller_.DeleteRequest(mobile_correlation_id);
}

void Ivdcm::ResetTimer(uint32_t mobile_correlation_id) {
  LOG4CXX_AUTO_TRACE(logger_);
  request_controller_.ResetTimer(mobile_correlation_id);
}

}  //  namespace ivdcm_module

