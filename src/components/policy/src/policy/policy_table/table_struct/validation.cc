#include "validation.h"  // NOLINT

#include <algorithm>
#include <locale>
#include <cctype>
#include <cstdlib>
#include <string>
#include <vector>
#include "utils/macro.h"

namespace rpc {
namespace policy_table_interface_base {
bool ApplicationParams::Validate() const {
  return ModuleTypesValidator().Validate(&moduleType);
}

bool ModuleTypesValidator::Validate(Optional<ModuleTypes>* module_types) const {
  DCHECK_OR_RETURN(module_types, false);
  Optional<ModuleTypes>& modules = *module_types;
  // moduleType is optional so see Optional<T>::is_valid()
  bool is_initialized = modules->is_initialized();
  if (!is_initialized) {
    // valid if not initialized
    return true;
  }
  bool is_valid = modules->is_valid();
  if (is_valid) {
    return true;
  }

  struct IsInvalid {
    bool operator()(Enum<ModuleType> item) const {
      return !item.is_valid();
    }
  };
  // cut invalid items
  modules->erase(std::remove_if(modules->begin(), modules->end(), IsInvalid()),
                 modules->end());
  bool empty = modules->empty();
  if (empty) {
    // set non initialized value
    ModuleTypes non_initialized;
    *module_types = Optional<ModuleTypes>(non_initialized);
  }
  return true;
}

bool RpcParameters::Validate() const {
  return true;
}
bool Rpcs::Validate() const {
  return true;
}

bool ModuleConfig::Validate() const {

  if (PT_PRELOADED == GetPolicyTableType()) {

    if (vehicle_make.is_initialized()) {
      return false;
    }
    if (vehicle_year.is_initialized()) {
      return false;
    }
    if (vehicle_model.is_initialized()) {
      return false;
    }
  }
  return true;
}

bool MessageString::Validate() const {
  return true;
}

bool MessageLanguages::Validate() const {
  if (PT_SNAPSHOT == GetPolicyTableType()) {
    return false;
  }
  return true;
}

bool ConsumerFriendlyMessages::Validate() const {
  if (PT_SNAPSHOT == GetPolicyTableType()) {
    return false;
  }
  return true;
}

bool ModuleMeta::Validate() const {
  return true;
}

bool AppLevel::Validate() const {
  if (PT_PRELOADED == GetPolicyTableType() ||
      PT_UPDATE == GetPolicyTableType()) {
    return false;
  }
  return true;
}

bool UsageAndErrorCounts::Validate() const {
  if (PT_PRELOADED == GetPolicyTableType() ||
      PT_UPDATE == GetPolicyTableType()) {
   return false;
  }
  return true;
}

bool DeviceParams::Validate() const {
  return true;
}
bool PolicyTable::Validate() const {
  for (ApplicationPolicies::const_iterator it = app_policies.begin();
       app_policies.end() != it; ++it) {
    if (kDeviceApp == it->first) {
      if (it->second.nicknames.is_initialized()) {
        initialization_state__ = kUninitialized;
        return false;
      }
      continue;
    }
  }
  if (PT_PRELOADED == GetPolicyTableType() ||
      PT_UPDATE == GetPolicyTableType()) {
    if (device_data.is_initialized()) {
      return false;
    }
  }
  return true;
}
bool Table::Validate() const {
  return true;
}

namespace {
static const int kLength = 4;
static const std::string kRemoteRpcs[] = {
    "ButtonPress",
    "GetInteriorVehicleDataCapabilities",
    "GetInteriorVehicleData",
    "SetInteriorVehicleData"
};

static const int kLengthRadio = 10;
static const std::string kRadioParameters[] = {
    "frequencyInteger",
    "frequencyFraction",
    "band",
    "rdsData",
    "availableHDs",
    "hdChannel",
    "signalStrength",
    "signalChangeThreshold",
    "radioEnable",
    "state"
};

static const int kLengthClimate = 10;
static const std::string kClimateParameters[] = {
    "fanSpeed",
    "currentTemp",
    "desiredTemp",
    "acEnable",
    "acMaxEnable",
    "circulateAirEnable",
    "autoModeEnable",
    "defrostZone",
    "dualModeEnable",
    "ventilationMode"
};

static const int kLengthAudio = 3;
static const std::string kAudioParameters[] = {
    "source",
    "audioVolume",
    "equalizerSettings"
};

static const int kLengthSeats = 11;
static const std::string kHmiParameters[] = {
    "displayMode",
    "temperatureUnit",
    "distanceUnit"
};

static const int kLengthHmi = 3;
static const std::string kSeatsParameters[] = {
    "cooledSeats",
    "cooledSeatLevel",
    "heatedSeat",
    "seatHorizontalPosition",
    "seatVerticalPosition",
    "seatAnglePosition",
    "backTiltPosition",
    "massageSeat",
    "massageSeatZone",
    "massageSeatLevel",
    "massageEnabled",
    "backLumbarPosition"
};
}  // namespace

AccessModulesValidator::Iterators AccessModulesValidator::GetModuleParameters(ModuleType module) const {
  const std::string *begin = 0;
  const std::string *end = 0;
  switch (module) {
    case MT_RADIO:
      begin = kRadioParameters;
      end = kRadioParameters + kLengthRadio;
      break;
    case MT_CLIMATE:
      begin = kClimateParameters;
      end = kClimateParameters + kLengthClimate;
      break;
    case MT_AUDIO:
      begin = kAudioParameters;
      end = kAudioParameters + kLengthAudio;
      break;
    case MT_SEATS:
      begin = kSeatsParameters;
      end = kSeatsParameters + kLengthSeats;
      break;
    case MT_HMI_SETTINGS:
      begin = kHmiParameters;
      end = kHmiParameters + kLengthHmi;
      break;
  }
  return std::make_pair(begin, end);
}

bool AccessModulesValidator::ValidateParameters(ModuleType module,
                                      const Strings& parameters) const {
  Iterators its = GetModuleParameters(module);
  const std::string *begin = its.first;
  const std::string *end = its.second;
  DCHECK(begin);
  DCHECK(end);
  for (Strings::const_iterator i = parameters.begin();
      i != parameters.end(); ++i) {
    std::string name = *i;
    bool found = std::find(begin, end, name) != end;
    if (!found) {
      return false;
    }
  }
  return true;
}

bool AccessModulesValidator::ValidateRemoteRpcs(ModuleType module,
                                      const RemoteRpcs& rpcs) const {
  for (RemoteRpcs::const_iterator i = rpcs.begin();
      i != rpcs.end(); ++i) {
    const std::string& name = i->first;
    const Strings& parameters = i->second;
    const std::string *begin = kRemoteRpcs;
    const std::string *end = kRemoteRpcs + kLength;
    bool found = std::find(begin, end, name) != end;
    if (!found || !ValidateParameters(module, parameters)) {
      return false;
    }
  }
  return true;
}

bool AccessModulesValidator::Validate(const AccessModules& modules) const {
  for (AccessModules::const_iterator i = modules.begin();
      i != modules.end(); ++i) {
    const std::string& name = i->first;
    const RemoteRpcs& rpcs = i->second;
    ModuleType module;
    if (!EnumFromJsonString(name, &module)
        || !ValidateRemoteRpcs(module, rpcs)) {
      return false;
    }
  }
  return true;
}

bool InteriorZone::Validate() const {
  return AccessModulesValidator().Validate(auto_allow)
      && AccessModulesValidator().Validate(driver_allow);
}

namespace {
struct IsDeniedChar {
  bool operator() (wchar_t c) {
    return c != '_' && !std::isalnum(c, std::locale(""));
  }
};
}  // namespace

bool ZonesValidator::ValidateNameZone(const std::string& name) const {
  if (name.empty()) {
    return false;
  }
  std::vector<wchar_t> wchars(name.length() + 1, L'\0');
  std::string current_locale = setlocale(LC_ALL, NULL);
  setlocale(LC_ALL, "");
  int n = mbstowcs(&(wchars.front()), name.c_str(), name.length());
  setlocale(LC_ALL, current_locale.c_str());
  if (n != -1) {
    std::vector<wchar_t>::iterator real_end = wchars.begin() + n;
    return std::find_if(wchars.begin(), real_end, IsDeniedChar()) == real_end;
  }
  return false;
}

bool ZonesValidator::Validate(const Zones& zones) const {
  for (Zones::const_iterator i = zones.begin(); i != zones.end(); ++i) {
    if (!ValidateNameZone(i->first)) {
      return false;
    }
  }
  return true;
}

bool Equipment::Validate() const {
  return ZonesValidator().Validate(zones);
}

}  // namespace policy_table_interface_base
}  // namespace rpc
