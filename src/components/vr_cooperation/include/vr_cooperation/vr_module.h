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

#ifndef SRC_COMPONENTS_VR_COOPERATION_INCLUDE_VR_COOPERATION_VR_MODULE_H_
#define SRC_COMPONENTS_VR_COOPERATION_INCLUDE_VR_COOPERATION_VR_MODULE_H_

#include "functional_module/generic_module.h"
#include "utils/macro.h"
#include "vr_cooperation/request_controller.h"
#include "vr_cooperation/vr_proxy.h"
#include "vr_cooperation/vr_proxy_listener.h"
#include "vr_cooperation/service_module.h"

namespace vr_hmi_api {
class ServiceMessage;
}  // namespace vr_hmi_api

namespace vr_cooperation {

/**
 * @brief VR Module plugin class
 */
class VRModule : public functional_modules::GenericModule,
    public VRProxyListener, public ServiceModule {
 public:
  VRModule();
  ~VRModule();
  /**
   * @brief get plugin information
   */
  virtual functional_modules::PluginInfo GetPluginInfo() const;

  /**
   * Starts routines of the plugin
   */
  virtual void Start();

  /**
   * Stops routines of the plugin
   */
  virtual void Stop();

  /**
   * @brief process message from mobile
   * @param msg message from mobile
   */
  virtual functional_modules::ProcessResult ProcessMessage(
      application_manager::MessagePtr msg);

  /**
   * @brief process json message from HMI
   * @param msg json message from HMI
   */
  virtual functional_modules::ProcessResult ProcessHMIMessage(
      application_manager::MessagePtr msg);

  /**
   * @brief Remove extension created for specified application
   * @param app_id application id
   */
  virtual void RemoveAppExtension(uint32_t app_id);

  /**
   * @brief Check registering app can be handled by plugin
   * @param app Application basis already create by Core
   */
  virtual bool IsAppForPlugin(application_manager::ApplicationSharedPtr app);

  /**
   * @brief Notify about change of HMILevel of plugin's app
   * @param app application with new HMILevel
   * @param old_level Old HMILevel of app
   */
  virtual void OnAppHMILevelChanged(
      application_manager::ApplicationSharedPtr app,
      mobile_apis::HMILevel::eType old_level);

  /**
   * @brief Handles removing (disconnecting) device
   * @param device removed
   */
  virtual void OnDeviceRemoved(const connection_handler::DeviceHandle& device);

  /**
   * @brief send gpb message to HMI
   * @param message gpb message that will be sent to HMI
   */
  virtual void SendMessageToHMI(const vr_hmi_api::ServiceMessage& message);

  /**
   * @brief send message to mobile
   * @param msg message to send
   */
  virtual void SendMessageToMobile(application_manager::MessagePtr msg);

  /**
   * Handles received message from HMI (Applink)
   * @param message is GPB message according with protocol
   */
  virtual void OnReceived(const vr_hmi_api::ServiceMessage& message);

  /**
   * Handles starting channel
   */
  virtual void OnReady();

  /**
   * Adds request from request controller
   * @param command pointer to command
   * @param correlation_id request id
   */
  virtual void RegisterRequest(int32_t correlation_id,
                               commands::Command* command);

  /**
   * Removed request from request controller
   * @param correlation_id request id
   */
  virtual void UnregisterRequest(int32_t correlation_id);

  /**
   * @brief IsServiceSupported return true if VR service is supported
   */
  virtual bool IsServiceSupported() const {
    return supported_;
  }

  /**
   * @brief EnableSupport
   */
  virtual void EnableSupport() {
    supported_ = true;
  }

  /**
   * @brief DisableSupport
   */
  virtual void DisableSupport() {
    supported_ = false;
  }

  /**
   * @brief Returns key of application that process VR requests.
   * @return mobile application connection key
   */
  virtual int32_t activated_connection_key() const {
    return activated_connection_key_;
  }

  /**
   * @brief Sets key of application that process VR requests. It happens
   *        when ActivateServiceRequest come from HU
   * @param connection_key key of activate application
   */
  virtual void ActivateService(int32_t connection_key) {
    activated_connection_key_ = connection_key;
  }

  /**
   * @brief Reset key of application that process VR requests. It happens
   *        when OnDeactivatedService notification come from HU
   */
  virtual void DeactivateService() {
    activated_connection_key_ = 0;
  }

  /**
   * @brief IsDefaultService return true if app is default for service
   * false otherwise
   * @param app_id application id
   */
  virtual bool IsDefaultService(int32_t app_id) const {
    return default_app_id_ == app_id;
  }

  /**
   * @brief SetDefaultService setter for default vr-service app id
   * @param app_id default vr-service app id
   */
  virtual void SetDefaultService(int32_t app_id) {
    default_app_id_ = app_id;
  }

  /**
   * @brief ResetDefaultService
   */
  virtual void ResetDefaultService() {
    default_app_id_ = 0;
  }

  /**
   * @brief Returns unique correlation ID for next HMI request
   */
  int32_t GetNextCorrelationID() {
    return service()->GetNextCorrelationID();
  }

 protected:
  /**
   * @brief Remove extension for all applications
   */
  virtual void RemoveAppExtensions();

 private:
  /**
   * @brief Subscribes plugin to mobie rpc messages
   */
  void SubcribeToRPCMessage();

  /**
   * @brief handle mobile message
   * @param msg mobile message
   */
  functional_modules::ProcessResult HandleMessage(
      application_manager::MessagePtr msg);

  /**
   * @brief Set mobile message type
   * @param msg mobile message
   */
  bool SetMessageType(application_manager::MessagePtr msg) const;

  /**
   * @brief send response message to mobile in case VRService is not supported
   * @param msg message to send
   */
  void SendUnsupportedServiceResponse(application_manager::MessagePtr msg);

  static const functional_modules::ModuleID kVRModuleID = 154;
  functional_modules::PluginInfo plugin_info_;
  request_controller::RequestController request_controller_;

  VRProxy proxy_;
  int32_t activated_connection_key_;
  int32_t default_app_id_;
  bool supported_;

  DISALLOW_COPY_AND_ASSIGN(VRModule);
};

EXPORT_FUNCTION(VRModule)

}  // namespace vr_cooperation

#endif  // SRC_COMPONENTS_VR_COOPERATION_INCLUDE_VR_COOPERATION_VR_MODULE_H_
