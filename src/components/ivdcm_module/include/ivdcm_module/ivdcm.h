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

#ifndef SRC_COMPONENTS_IVDCM_MODULE_INCLUDE_IVDCM_MODULE_IVDCM_H_
#define SRC_COMPONENTS_IVDCM_MODULE_INCLUDE_IVDCM_MODULE_IVDCM_H_

#include <queue>
#include "functional_module/generic_module.h"
#include "json/value.h"
#include "ivdcm_module/ivdcm_proxy.h"
#include "ivdcm_module/ivdcm_proxy_listener.h"
#include "ivdcm_module/request_controller.h"
#include "utils/macro.h"

namespace ivdcm_module {

typedef Json::Value MessageFromMobile;

class Ivdcm
    : public functional_modules::GenericModule,
      public IvdcmProxyListener {
 public:
  Ivdcm();
  ~Ivdcm();
  virtual functional_modules::PluginInfo GetPluginInfo() const;
  virtual functional_modules::ProcessResult ProcessHMIMessage(
      application_manager::MessagePtr msg);

  /**
   * @brief Process messages from mobile(called from SDL part through interface)
   * @param msg request mesage
   * @return processing result
   */
  virtual functional_modules::ProcessResult ProcessMessage(
    application_manager::MessagePtr msg);
  virtual void RemoveAppExtension(uint32_t app_id);
  virtual void OnDeviceRemoved(const connection_handler::DeviceHandle& device);
  virtual void RemoveAppExtensions();
  virtual bool IsAppForPlugin(application_manager::ApplicationSharedPtr app);
  virtual void OnAppHMILevelChanged(application_manager::ApplicationSharedPtr app,
      mobile_apis::HMILevel::eType old_level);

  virtual void OnReceived(const sdl_ivdcm_api::SDLRPC &message);
  void SendIvdcmMesssage(const sdl_ivdcm_api::SDLRPC &message);

  /**
   * @brief Sends message to mobile application
   * @param msg message
   */
  void SendMessageToMobile(application_manager::MessagePtr msg);

  // RequestController wrapped methods
  void AddRequestToRequestController(uint32_t mobile_correlation_id,
                                  request_controller::MobileRequestPtr request);
  void DeleteRequestFromRequestController( uint32_t mobile_correlation_id);
  void ResetTimer(uint32_t mobile_correlation_id);

  /**
   * @brief Returns unique correlation ID for request to mobile
   *
   * @return Unique correlation ID
   */
  static uint32_t GetNextCorrelationID() {
    return next_correlation_id_++;
  }

  /**
   * @brief Returns key of application that process ivdcm requests.
   * @return mobile application connection key
   */
  uint32_t connection_key() const {
    return connection_key_;
  }

  /**
   * @brief Sets key of application that process ivdcm requests. It happens
   *        when OnInternateStateNotification come from mobile.
   */
  void set_connection_key(const uint32_t& connection_key) {
    connection_key_ = connection_key;
  }

 private:

  /**
   * @brief Subscribes plugin to mobie rpc messages
   */
  void SubscribeToRpcMessages();

 private:
  static const functional_modules::ModuleID kModuleID = 404;
  functional_modules::PluginInfo plugin_info_;
  IvdcmProxy proxy_;
  // TODO(KKolodiy): this tun_id_ was add for checking TUN adapter only
  // should be removed after implementation of the domain logic
  int tun_id_;
  uint32_t connection_key_;

  static uint32_t next_correlation_id_;

  request_controller::RequestController request_controller_;


  friend class IvdcmModuleTest;
  DISALLOW_COPY_AND_ASSIGN(Ivdcm);
};

EXPORT_FUNCTION(Ivdcm)

}  // namespace ivdcm_module

#endif  // SRC_COMPONENTS_IVDCM_MODULE_INCLUDE_IVDCM_MODULE_IVDCM_H_
