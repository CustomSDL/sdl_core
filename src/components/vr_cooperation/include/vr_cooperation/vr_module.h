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

namespace vr_cooperation {
class VRProxy;

/**
 * @brief VR Module plugin class
 */
class VRModule : public functional_modules::GenericModule {
 public:
  VRModule();
  ~VRModule();
  /**
   * @brief get plugin information
   */
  functional_modules::PluginInfo GetPluginInfo() const;

  /**
   * @brief process message from mobile
   * @param msg message from mobile
   */
  virtual functional_modules::ProcessResult ProcessMessage(
      application_manager::MessagePtr msg);

  /**
   * @brief process message from HMI
   * @param msg message from HMI
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
  bool IsAppForPlugin(application_manager::ApplicationSharedPtr app);

  /**
   * @brief Notify about change of HMILevel of plugin's app
   * @param app application with new HMILevel
   * @param old_level Old HMILevel of app
   */
  void OnAppHMILevelChanged(application_manager::ApplicationSharedPtr app,
                            mobile_apis::HMILevel::eType old_level);

  /**
   * @brief Handles removing (disconnecting) device
   * @param device removed
   */
  void OnDeviceRemoved(const connection_handler::DeviceHandle& device);

  /**
   * @brief send message to HMI
   * @param msg message that will be sent to HMI
   */
  void SendMessageToHMI(application_manager::MessagePtr msg);

  /**
   * @brief receive message from HMI
   */
  void ReceiveMessageFromHMI();

  /**
   * @brief send message to mobile
   * @param msg message to send
   */
  void SendMessageToMobile(application_manager::MessagePtr msg);

  /**
   * @brief receive message from mobile
   */
  void ReceiveMessageFromMobile();

  /**
   * @brief check if VR Service support or not
   */
  bool IsVRServiceSupported();

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
   * @brief handle HMI message
   * @param msg HMI message
   */
  functional_modules::ProcessResult HandleHMIMessage(
      application_manager::MessagePtr msg);

  /**
   * @brief Set mobile message type
   * @param msg mobile message
   */
  bool SetMessageType(application_manager::MessagePtr& msg) const;

  /**
   * @brief Set HMI message type
   * @param msg hmi message
   */
  bool SetHMIMessageType(application_manager::MessagePtr& msg) const;

  static const functional_modules::ModuleID kVRModuleID = 154;
  functional_modules::PluginInfo plugin_info_;
  request_controller::RequestController request_controller_;

  VRProxy* proxy_;

  DISALLOW_COPY_AND_ASSIGN(VRModule);
};

EXPORT_FUNCTION(VRModule)

}  // namespace vr_cooperation

#endif  // SRC_COMPONENTS_VR_COOPERATION_INCLUDE_VR_COOPERATION_VR_MODULE_H_
