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

#include "can_cooperation/validators/struct_validators/hmi_control_data_validator.h"
#include "can_cooperation/can_module_constants.h"

namespace can_cooperation {

namespace validators {

CREATE_LOGGERPTR_GLOBAL(logger_, "HMIControlDataValidator")

using message_params::kDisplayMode;
using message_params::kTemperatureUnit;
using message_params::kDistanceUnit;

HMIControlDataValidator::HMIControlDataValidator() {

  // name="displayMode"
  display_mode_[ValidationParams::TYPE] = ValueType::ENUM;
  display_mode_[ValidationParams::ENUM_TYPE] = EnumType::DISPLAY_MODE;
  display_mode_[ValidationParams::ARRAY] = 0;
  display_mode_[ValidationParams::MANDATORY] = 0;

  // name="temperatureUnit"
  temperature_unit_[ValidationParams::TYPE] = ValueType::ENUM;
  temperature_unit_[ValidationParams::ENUM_TYPE] = EnumType::TEMPERATURE_UNIT;
  temperature_unit_[ValidationParams::ARRAY] = 0;
  temperature_unit_[ValidationParams::MANDATORY] = 0;

  // name="distanceUnit"
  distance_unit_[ValidationParams::TYPE] = ValueType::ENUM;
  distance_unit_[ValidationParams::ENUM_TYPE] = EnumType::DISTANCE_UNIT;
  distance_unit_[ValidationParams::ARRAY] = 0;
  distance_unit_[ValidationParams::MANDATORY] = 0;

  validation_scope_map_[kDisplayMode] = &display_mode_;
  validation_scope_map_[kTemperatureUnit] = &temperature_unit_;
  validation_scope_map_[kDistanceUnit] = &distance_unit_;
}

ValidationResult HMIControlDataValidator::Validate(const Json::Value& json,
                                                 Json::Value& outgoing_json) {
  LOG4CXX_AUTO_TRACE(logger_);

  if (!json.isObject()) {
    LOG4CXX_ERROR(logger_, "HMIControlData must be struct");
    return INVALID_DATA;
  }

  ValidationResult result = ValidateSimpleValues(json, outgoing_json);

  if (!outgoing_json.size()) {
    result = INVALID_DATA;
  }

  return result;
}

}  // namespace validators

}  // namespace can_cooperation

