/*
 Copyright (c) 2013, Ford Motor Company
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

#ifndef SRC_COMPONENTS_POLICY_INCLUDE_POLICY_POLICY_MANAGER_H_
#define SRC_COMPONENTS_POLICY_INCLUDE_POLICY_POLICY_MANAGER_H_

#include <vector>

#include "policy/policy_types.h"
#include "policy/policy_listener.h"
#include "usage_statistics/statistics_manager.h"
#include "policy/access_remote.h"

namespace policy {

class PolicyManager : public usage_statistics::StatisticsManager {
  public:
    virtual ~PolicyManager() {
    }

    virtual void set_listener(PolicyListener* listener) = 0;

    /**
     * Inits Policy Table
     * @param file_name Path to preloaded PT file
     * @return true if successfully
     */
    virtual bool InitPT(const std::string& file_name) = 0;

    /**
     * @brief Updates Policy Table from binary message received from
     * mobile device. Saves to Policy Table diff between Policy Table
     * sent in snapshot and received Policy Table.
     * @param file name of file with update policy table
     * @param pt_content PTU as binary string
     * @return bool Success of operation
     */
    virtual bool LoadPT(const std::string& file, const BinaryMessage& pt_content) = 0;

    /**
     * Resets Policy Table
     * @param file_name Path to preloaded PT file
     * @return true if successfully
     */
    virtual bool ResetPT(const std::string& file_name) = 0;

    /**
     * @brief GetLockScreenIcon allows to obtain lock screen icon url;
     *
     * @return url which point to the resourse where lock screen icon could be obtained.
     */
    virtual std::string GetLockScreenIconUrl() const = 0;

    /**
     * @brief Gets all URLs for sending PTS to from PT itself.
     * @param service_type Service specifies user of URL
     * @return vector of urls
     */
    virtual void GetServiceUrls(const std::string& service_type,
                                EndpointUrls& end_points) = 0;

    /**
     * @brief PTU is needed, for this PTS has to be formed and sent.
     */
    virtual void RequestPTUpdate() = 0;

    /**
     * @brief Check if specified RPC for specified application
     * has permission to be executed in specified HMI Level
     * and also its permitted params.
     * @param device_id unique identifier of device
     * @param app_id Id of application provided during registration
     * @param hmi_level Current HMI Level of application
     * @param rpc Name of RPC
     * @param CheckPermissionResult containing flag if HMI Level is allowed
     * and list of allowed params.
     */
    virtual void CheckPermissions(const PTString& device_id,
        const PTString& app_id,
        const PTString& hmi_level,
        const PTString& rpc,
        const RPCParams& rpc_params,
        CheckPermissionResult& result) = 0;

    /**
     * @brief Clear all record of user consents. Used during Factory Reset.
     * @return bool Success of operation
     */
    virtual bool ResetUserConsent() = 0;

    /**
     * @brief Returns current status of policy table for HMI
     * @return Current status of policy table
     */
    virtual std::string GetPolicyTableStatus() const = 0;

    /**
     * Checks is PT exceeded kilometers
     * @param kilometers current kilometers at odometer
     * @return true if exceeded
     */
    virtual void KmsChanged(int kilometers) = 0;

    /**
     * Increments counter of ignition cycles
     */
    virtual void IncrementIgnitionCycles() = 0;

    /**
     * @brief ExchangeByUserRequest
     */
    virtual std::string ForcePTExchange() = 0;

    /**
     * Resets retry sequence
     */
    virtual void ResetRetrySequence() = 0;

    /**
     * Gets timeout to wait before next retry updating PT
     * If timeout is less or equal to zero then the retry sequence is not need.
     * @return timeout in seconds
     */
    virtual int NextRetryTimeout() = 0;

    /**
     * Gets timeout to wait until receive response
     * @return timeout in seconds
     */
    virtual int TimeoutExchange() = 0;

    /**
     * @brief List of timeouts in seconds between retries
     *        when attempt to update PT fails
     * @return List of delays between attempts.
     */
    virtual const std::vector<int> RetrySequenceDelaysSeconds() = 0;

    /**
     * Handler of exceeding timeout of exchanging policy table
     */
    virtual void OnExceededTimeout() = 0;

    /**
     * @brief Handler of PTS sending out
     */
    virtual void OnUpdateStarted() = 0;

    /**
     * @brief Check user consent for mobile device data connection
     * @param device_id Unique device identifier
     * @return status of device consent
     */
    virtual DeviceConsent GetUserConsentForDevice(
      const std::string& device_id) = 0;

    /**
     * @brief Get user consent for application
     * @param device_id Device id
     * @param policy_app_id Unique application id
     * @param permissions Array of functional groups permissions
     */
    virtual void GetUserConsentForApp(
      const std::string& device_id, const std::string& policy_app_id,
      std::vector<FunctionalGroupPermission>& permissions) = 0;

    /**
     * @brief Set user consent for mobile device data connection
     * @param device_id Unique device identifier
     * @param is_allowed User consent for usage device data connection
     */
    virtual void SetUserConsentForDevice(const std::string& device_id,
                                         bool is_allowed) = 0;

    /**
     * @brief Update Application Policies as reaction
     * on User allowing/disallowing device this app is running on.
     */
    virtual bool ReactOnUserDevConsentForApp(const std::string app_id,
        bool is_device_allowed) = 0;
    /**
     * Sets number of kilometers and days after epoch, that passed for
     * receiving PT UPdate.
     */
    virtual void PTUpdatedAt(int kilometers, int days_after_epoch) = 0;

    /**
     * @brief Retrieves data from app_policies about app on its registration:
     * @param app_id - id of registered app
     * @param app_types Section on HMI where app can appear (Navigation, Phone etc)
     * @param nicknames Synonyms for application
     */
    virtual bool GetInitialAppData(const std::string& application_id,
                                   StringArray* nicknames = NULL,
                                   StringArray* app_hmi_types = NULL) = 0;
    /**
     * @brief Stores device parameters received during application registration
     * to policy table
     * @param device_id Device mac address
     * @param device_info Received device parameters
     */
    virtual void SetDeviceInfo(const std::string& device_id,
                               const DeviceInfo& device_info) = 0;

    /**
     * @brief Set user consent for application functional groups
     * @param permissions User-defined application group pemissions.
     * The permissions is not const reference because it may contains
     * valid data as well as invalid. So we will remove all invalid data
     * from this structure.
     */
    virtual void SetUserConsentForApp(const PermissionConsent& permissions) = 0;

    /**
     * @brief Get default HMI level for application
     * @param policy_app_id Unique application id
     * @param default_hmi Default HMI level for application or empty, if value
     * was not set
     * @return true, if succedeed, otherwise - false
     */
    virtual bool GetDefaultHmi(const std::string& policy_app_id,
                               std::string* default_hmi) = 0;

    /**
     * @brief Get priority for application
     * @param policy_app_id Unique application id
     * @param priority Priority for application or empty, if value was not set
     * @return true, if succedeed, otherwise - false
     */
    virtual bool GetPriority(const std::string& policy_app_id,
                             std::string* priority) = 0;

    /**
     * @brief Get user friendly messages for given RPC messages and language
     * @param message_codes RPC message codes
     * @param language Language
     * @return Array of structs with appropriate message parameters
     */
    virtual std::vector<UserFriendlyMessage> GetUserFriendlyMessages(
      const std::vector<std::string>& message_code,
      const std::string& language) = 0;

    /**
     * Checks if the application is revoked
     * @param app_id application id
     * @return true if application is revoked
     */
    virtual bool IsApplicationRevoked(const std::string& app_id) const = 0;

    /**
     * @brief Get resulting RPCs permissions for application which started on
     * specific device
     * @param device_id Device id
     * @param policy_app_id Unique application id
     * @param permissions Array of functional groups permissions
     */
    virtual void GetPermissionsForApp(
      const std::string& device_id, const std::string& policy_app_id,
      std::vector<FunctionalGroupPermission>& permissions) = 0;

    /**
     * @brief Gets specific application permissions changes since last policy
     * table update
     * @param policy_app_id Unique application id
     * @return Permissions changes
     */
    virtual AppPermissions GetAppPermissionsChanges(
            const std::string& policy_app_id) = 0;

    virtual void RemovePendingPermissionChanges(const std::string& app_id) = 0;

    /**
     * @brief Return device id, which hosts specific application
     * @param Application id, which is required to update device id
     * @deprecated
     */
    virtual std::string& GetCurrentDeviceId(const std::string& policy_app_id) = 0;

    /**
     * @brief Set current system language
     * @param language Language
     */
    virtual void SetSystemLanguage(const std::string& language) = 0;

    /**
     * @brief Set data from GetSystemInfo response to policy table
     * @param ccpu_version CCPU version
     * @param wers_country_code WERS country code
     * @param language System language
     */
    virtual void SetSystemInfo(const std::string& ccpu_version,
                               const std::string& wers_country_code,
                               const std::string& language) = 0;

    /**
     * @brief Send OnPermissionsUpdated for choosen application
     * @param device_id
     * @param application_id
     */
    virtual void SendNotificationOnPermissionsUpdated(
       const std::string& device_id, const std::string& application_id) = 0;

    /**
     * Marks device as upaired
     * @param device_id id device
     */
    virtual void MarkUnpairedDevice(const std::string& device_id) = 0;

    /**
     * @brief Adds, application to the db or update existed one
     * run PTU if policy update is necessary for application.
     * @param device_id unique identifier of device
     * @param Application id assigned by Ford to the application
     * @param hmi_types list of hmi types
     */
    virtual void AddApplication(const std::string& device_id,
                                const std::string& application_id,
                                const std::vector<int>& hmi_types) = 0;

    /**
     * @brief Removes unpaired device records and related records from DB
     * @param device_ids List of device_id, which should be removed
     * @return true, if succedeed, otherwise - false
     */
    virtual bool CleanupUnpairedDevices() = 0;

    /**
     * @brief Check if app can keep context.
     */
    virtual bool CanAppKeepContext(const std::string& app_id) = 0;

    /**
     * @brief Check if app can steal focus.
     */
    virtual bool CanAppStealFocus(const std::string& app_id) = 0;

    /**
     * @brief Runs necessary operations, which is depends on external system
     * state, e.g. getting system-specific parameters which are need to be
     * filled into policy table
     */
    virtual void OnSystemReady() = 0;

    /**
     * @brief GetNotificationNumber
     * @param priority
     * @return
     */
    virtual uint32_t GetNotificationsNumber(const std::string& priority) = 0;

    /**
     * @brief Allows to update Vehicle Identification Number in policy table.
     * @param new value for the parameter.
     */
    virtual void SetVINValue(const std::string& value) = 0;

    /**
     * @brief Checks, if application has policy assigned w/o data consent
     * @param policy_app_id Unique application id
     * @return true, if policy assigned w/o data consent, otherwise -false
     */
    virtual bool IsPredataPolicy(const std::string& policy_app_id) = 0;

    /**
     * Gets HMI types
     * @param application_id ID application
     * @param app_types list to save HMI types
     * @return true if policy has specific policy for this application
     */
    virtual bool GetHMITypes(const std::string& application_id,
                             std::vector<int> *app_types) = 0;

    /**
     * Returns heart beat timeout
     * @param app_id application id
     * @return if timeout was set then value in seconds greater zero
     * otherwise heart beat for specific application isn't set
     */
    virtual uint16_t HeartBeatTimeout(const std::string& app_id) const = 0;

    /**
     * @brief SaveUpdateStatusRequired alows to save update status.
     */
    virtual void SaveUpdateStatusRequired(bool is_update_needed) = 0;

    /**
     * @brief Handler on applications search started
     */
    virtual void OnAppsSearchStarted() = 0;

    /**
     * @brief Handler on applications search completed
     */
    virtual void OnAppsSearchCompleted() = 0;

    /**
     * @brief OnAppRegisteredOnMobile alows to handle event when application were
     * succesfully registered on mobile device.
     * It will send OnAppPermissionSend notification and will try to start PTU.
     * @param device_id unique identifier of device
     * @param application_id registered application.
     */
    virtual void OnAppRegisteredOnMobile(const std::string& device_id,
        const std::string& application_id) = 0;

#ifdef SDL_REMOTE_CONTROL
    /**
     * Checks access to equipment of vehicle for application by RPC
     * @param device_id unique identifier of device
     * @param app_id policy id application
     * @param zone control
     * @param module
     * @param rpc name of rpc
     * @param params parameters list
     */
    virtual TypeAccess CheckAccess(const PTString& device_id,
                                   const PTString& app_id,
                                   const SeatLocation& zone,
                                   const PTString& module,
                                   const PTString& rpc,
                                   const RemoteControlParams& params) = 0;

    /**
     * Checks access to module for application
     * @param app_id policy id application
     * @param module
     * @return true if module is allowed for application
     */
    virtual bool CheckModule(const PTString& app_id,
                             const PTString& module) = 0;

    /**
     * Sets access to equipment of vehicle for application by RPC
     * @param dev_id unique identifier of device
     * @param app_id policy id application
     * @param zone control
     * @param module type
     * @param allowed true if access is allowed
     */
    virtual void SetAccess(const PTString& dev_id, const PTString& app_id,
                           const SeatLocation& zone,
                           const PTString& module, bool allowed) = 0;

    /**
     * Resets access application to all resources
     * @param dev_id unique identifier of device
     * @param app_id policy id application
     */
    virtual void ResetAccess(const PTString& dev_id,
                             const PTString& app_id) = 0;

    /**
     * Resets access by functional group for all applications
     * @param zone control
     * @param module type
     */
    virtual void ResetAccess(const SeatLocation& zone,
                             const PTString& module) = 0;

    /**
     * Sets driver as primary device
     * @param dev_id ID device
     */
    virtual void SetPrimaryDevice(const PTString& dev_id) = 0;

    /**
     * Resets driver's device
     */
    virtual void ResetPrimaryDevice() = 0;

    /**
     * Gets current primary device
     * @return ID device
     */
    virtual PTString PrimaryDevice() const = 0;

    /**
     * Sets device zone
     * @param dev_id unique identifier of device
     * @param zone device zone
     */
    virtual void SetDeviceZone(const PTString& dev_id,
                               const SeatLocation& zone) = 0;

    /**
     * Gets device zone
     * @param dev_id unique identifier of device
     * @param zone value to return zone if device has zone
     * @return true if device has zone
     */
    virtual bool GetDeviceZone(const PTString& dev_id,
                               SeatLocation* zone) const = 0;

    /**
     * Sets mode of remote control (on/off)
     * @param enabled true if remote control is turned on
     */
    virtual void SetRemoteControl(bool enabled) = 0;

    /*
     * @brief If remote control is enabled
     * by User and by Policy
     */
    virtual bool GetRemoteControl() const = 0;

    /**
     * Handles changed primary device event for a application
     * @param device_id Device on which app is running
     * @param application_id ID application
     */
    virtual void OnChangedPrimaryDevice(const std::string& device_id,
                                        const std::string& application_id) = 0;

    /**
     * Handles changed remote control event for a application
     * @param device_id Device on which app is running
     * @param application_id ID application
     */
    virtual void OnChangedRemoteControl(const std::string& device_id,
                                        const std::string& application_id) = 0;

    /**
     * Handles changed device zone event for a application
     * @param device_id Device on which app is running
     * @param application_id ID application
     */
    virtual void OnChangedDeviceZone(const std::string& device_id,
                                     const std::string& application_id) = 0;

    /*
     * Send OnPermissionsChange notification to mobile app
     * when it's permissions are changed.
     * @param device_id Device on which app is running
     * @param application_id ID of app whose permissions are changed
     */
    virtual void SendAppPermissionsChanged(const std::string& device_id,
      const std::string& application_id) = 0;

    /**
     * Gets all allowed module types
     * @param app_id unique identifier of application
     * @param list of allowed module types
     * @return true if application has allowed modules
     */
    virtual bool GetModuleTypes(const std::string& policy_app_id,
                                std::vector<std::string>* modules) const = 0;
#endif  // SDL_REMOTE_CONTROL

  protected:
    /**
     * Checks is PT exceeded IgnitionCycles
     * @return true if exceeded
     */
    virtual bool ExceededIgnitionCycles() = 0;

    /**
     * Checks is PT exceeded days
     * @return true if exceeded
     */
    virtual bool ExceededDays() = 0;

    /**
     * @brief StartPTExchange allows to start PTU. The function will check
     * if one is required and starts the update flow in only case when previous
     * condition is true.
     */
    virtual void StartPTExchange() = 0;
};

}  // namespace policy

extern "C" policy::PolicyManager* CreateManager();

#endif  // SRC_COMPONENTS_POLICY_INCLUDE_POLICY_POLICY_MANAGER_H_
