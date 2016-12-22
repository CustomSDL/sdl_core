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

#include <algorithm>

#include "can_cooperation/validators/validator.h"
#include "can_cooperation/can_module_constants.h"
#include "can_cooperation/message_helper.h"
#include "utils/logger.h"

namespace can_cooperation {

namespace validators {

CREATE_LOGGERPTR_GLOBAL(logger_, "Validator")

ValidationResult Validator::Validate(const Json::Value& json,
                                     Json::Value& outgoing_json) {
  LOG4CXX_AUTO_TRACE(logger_);
  // VS: created non-pure virtual for unit tests
  return ValidationResult::SUCCESS;
}

ValidationResult Validator::ValidateSimpleValues(const Json::Value& json,
                                       Json::Value& outgoing_json) {
  ValidationScopeMap::iterator it = validation_scope_map_.begin();

  ValidationResult result = ValidationResult::INVALID_DATA;

  for (; it != validation_scope_map_.end(); ++it) {
    result = ValidateValue(it->first, json, outgoing_json, *(it->second));

    if (ValidationResult::SUCCESS != result) {
      return result;
    }
  }

  if (ValidationResult::SUCCESS != result) {
    LOG4CXX_ERROR(logger_, "Validation scope map empty!");
  }

  return result;
}

ValidationResult Validator::ValidateValue(const std::string& value_name,
                                 const Json::Value& json,
                                 Json::Value& outgoing_json,
                                 ValidationScope& validation_scope) {
  // Check if param exist, and its mandatory
  if (!IsMember(json, value_name)) {
    if (validation_scope[ValidationParams::MANDATORY]) {
      LOG4CXX_ERROR(logger_, "Mandatory param " <<value_name <<" missing!");
      return ValidationResult::INVALID_DATA;
    } else {
      return ValidationResult::SUCCESS;
    }
  }

  ValidationResult result = ValidationResult::INVALID_DATA;

  // Check if param array. If it is array check array borders
  if (validation_scope[ValidationParams::ARRAY]) {
    if (!json[value_name].isArray()) {
      LOG4CXX_ERROR(logger_, value_name <<" must be array!");
      return ValidationResult::INVALID_DATA;
    }

    int size = json[value_name].size();

    if ((validation_scope[ValidationParams::MAX_SIZE] < size) ||
        (validation_scope[ValidationParams::MIN_SIZE] > size)) {
      LOG4CXX_ERROR(logger_, value_name <<" array wrong size!");
      return ValidationResult::INVALID_DATA;
    }

    for (int i = 0; i < size; ++i) {
      LOG4CXX_INFO(logger_, value_name <<" " <<i <<" array member validation.");
      result = Validate(json[value_name][i], validation_scope);

      if (ValidationResult::SUCCESS != result) {
        return result;
      }

      outgoing_json[value_name][i] = json[value_name][i];
    }
  } else {
    LOG4CXX_INFO(logger_, value_name <<" validation.");
    result = Validate(json[value_name], validation_scope);

    if (ValidationResult::SUCCESS != result) {
      return result;
    }

    outgoing_json[value_name] = json[value_name];
  }

  return ValidationResult::SUCCESS;
}

ValidationResult Validator::Validate(const Json::Value& json,
                                     ValidationScope& validation_scope) {
  if (ValueType::STRING == validation_scope[ValidationParams::TYPE]) {
    if (!json.isString()) {
      LOG4CXX_ERROR(logger_, "Must be string!");
      return ValidationResult::INVALID_DATA;
    }

    return ValidateStringValue(json.asCString(), validation_scope);
  } else if (ValueType::INT == validation_scope[ValidationParams::TYPE]) {
    if (!json.isInt()) {
      LOG4CXX_ERROR(logger_, "Must be integer!");
      return ValidationResult::INVALID_DATA;
    }

    return ValidateIntValue(json.asInt(), validation_scope);
  } else if (ValueType::DOUBLE == validation_scope[ValidationParams::TYPE]) {
    if ((!json.isDouble()) && (!json.isInt())) {      // Mobile SDK may to cut the floating point in some cases
      LOG4CXX_ERROR(logger_, "Must be fractional!");  // for exmple: "35.0", "1.0". So it would be parsed like Integer
      return ValidationResult::INVALID_DATA;          // by json reader.
    }

    return ValidateDoubleValue(json.asDouble(), validation_scope);
  } else if (ValueType::BOOL == validation_scope[ValidationParams::TYPE]) {
    if (!json.isBool()) {
      LOG4CXX_ERROR(logger_, "Must be boolean!");
            return ValidationResult::INVALID_DATA;
    }

    return ValidationResult::SUCCESS;
  } else if (ValueType::ENUM == validation_scope[ValidationParams::TYPE]) {
    if (!json.isString()) {
      LOG4CXX_ERROR(logger_,
                    "Enum field must be represented as string in json!");
      return ValidationResult::INVALID_DATA;
    }

    return ValidateEnumValue(json.asCString(), validation_scope);
  } else {
    return ValidationResult::INVALID_DATA;
  }
}

ValidationResult Validator::ValidateIntValue(int value,
                                            ValidationScope& validation_scope) {
  if ((value < validation_scope[ValidationParams::MIN_VALUE]) ||
      (value > validation_scope[ValidationParams::MAX_VALUE])) {
    LOG4CXX_ERROR(logger_, "Out of scope!");
    return ValidationResult::INVALID_DATA;
  }

  return ValidationResult::SUCCESS;
}

ValidationResult Validator::ValidateDoubleValue(double value,
                                            ValidationScope& validation_scope) {
  if ((value < validation_scope[ValidationParams::MIN_VALUE]) ||
      (value > validation_scope[ValidationParams::MAX_VALUE])) {
    LOG4CXX_ERROR(logger_, "Out of scope!");
    return ValidationResult::INVALID_DATA;
  }

  return ValidationResult::SUCCESS;
}

ValidationResult Validator::ValidateStringValue(const std::string& value,
                                            ValidationScope& validation_scope) {
  int size = value.size();
  if ((size < validation_scope[ValidationParams::MIN_LENGTH]) ||
      (size > validation_scope[ValidationParams::MAX_LENGTH])) {
    LOG4CXX_ERROR(logger_, "Out of scope!");
    return ValidationResult::INVALID_DATA;
  }

  return ValidationResult::SUCCESS;
}

const Validator::ValuesEnumMap Validator::values_ = Validator::PrepareEnumValidator();

Validator::ValuesEnumMap Validator::PrepareEnumValidator() {
  ValuesEnumMap values;
  values[EnumType::BUTTON_NAME] = PrepareButtonName();
  values[EnumType::MODULE_TYPE] = PrepareModuleType();
  values[EnumType::RADIO_BAND] = PrepareRadioBand();
  values[EnumType::RADIO_STATE] = PrepareRadioState();
  values[EnumType::DEFROST_ZONE] = PrepareDefrostZone();
  values[EnumType::BUTTON_PRESS_MODE] = PrepareButtonPressMode();
  values[EnumType::VENTILATION_MODE] = PrepareVentilationMode();
  values[EnumType::DISPLAY_MODE]= PrepareDisplayMode();
  values[EnumType::DISTANCE_UNIT] = PrepareDistanceUnit();
  values[EnumType::TEMPERATURE_UNIT] = PrepareTemperatureUnit();
  values[EnumType::LUMBAR_POSITION] = PrepareLumbarPosition();
  values[EnumType::MASSAGE_SEAT_ACTION] = PrepareMassageSeatAction();
  values[EnumType::MASSAGE_SEAT_ZONE] = PrepareMassageSeatZone();
  values[EnumType::MASSAGE_SEAT_LEVEL] = PrepareMassageSeatLevel();
  values[EnumType::AUDIO_SOURCE] = PrepareAudioSource();
  return values;
}

Validator::ValuesEnum Validator::PrepareButtonName() {
  ValuesEnum enums;
  enums.push_back(enums_value::kACMax);
  enums.push_back(enums_value::kAC);
  enums.push_back(enums_value::kRecirculate);
  enums.push_back(enums_value::kFanUp);
  enums.push_back(enums_value::kFanDown);
  enums.push_back(enums_value::kTempUp);
  enums.push_back(enums_value::kTempDown);
  enums.push_back(enums_value::kDefrostMax);
  enums.push_back(enums_value::kDefrost);
  enums.push_back(enums_value::kDefrostRear);
  enums.push_back(enums_value::kUpperVent);
  enums.push_back(enums_value::kLowerVent);
  enums.push_back(enums_value::kVolumeUp);
  enums.push_back(enums_value::kVolumeDown);
  enums.push_back(enums_value::kEject);
  enums.push_back(enums_value::kSource);
  enums.push_back(enums_value::kShuffle);
  enums.push_back(enums_value::kRepeat);
  return enums;
}

Validator::ValuesEnum Validator::PrepareModuleType() {
  ValuesEnum enums;
  enums.push_back(enums_value::kRadio);
  enums.push_back(enums_value::kAuido);
  enums.push_back(enums_value::kClimate);
  enums.push_back(enums_value::kSeats);
  enums.push_back(enums_value::kHmiSettings);
  return enums;
}

Validator::ValuesEnum Validator::PrepareRadioBand() {
  ValuesEnum enums;
  enums.push_back(enums_value::kAM);
  enums.push_back(enums_value::kFM);
  enums.push_back(enums_value::kXM);
  return enums;
}

Validator::ValuesEnum Validator::PrepareRadioState() {
  ValuesEnum enums;
  enums.push_back(enums_value::kAcquiring);
  enums.push_back(enums_value::kAcquired);
  enums.push_back(enums_value::kNotFound);
  enums.push_back(enums_value::kMulticast);
  return enums;
}

Validator::ValuesEnum Validator::PrepareDefrostZone() {
  ValuesEnum enums;
  enums.push_back(enums_value::kFront);
  enums.push_back(enums_value::kRear);
  enums.push_back(enums_value::kAll);
  return enums;
}

Validator::ValuesEnum Validator::PrepareButtonPressMode() {
  ValuesEnum enums;
  enums.push_back(enums_value::kLong);
  enums.push_back(enums_value::kShort);
  return enums;
}

Validator::ValuesEnum Validator::PrepareVentilationMode() {
  ValuesEnum enums;
  enums.push_back(enums_value::kUpper);
  enums.push_back(enums_value::kLower);
  enums.push_back(enums_value::kBoth);
  return enums;
}

Validator::ValuesEnum Validator::PrepareDisplayMode() {
  ValuesEnum enums;
  enums.push_back(enums_value::kDay);
  enums.push_back(enums_value::kNight);
  enums.push_back(enums_value::kAuto);
  return enums;
}

Validator::ValuesEnum Validator::PrepareDistanceUnit() {
  ValuesEnum enums;
  enums.push_back(enums_value::kMiles);
  enums.push_back(enums_value::kKilometers);
  return enums;
}

Validator::ValuesEnum Validator::PrepareTemperatureUnit() {
  ValuesEnum enums;
  enums.push_back(enums_value::kFahrenheit);
  enums.push_back(enums_value::kCelsius);
  return enums;
}

Validator::ValuesEnum Validator::PrepareLumbarPosition() {
  ValuesEnum enums;
  enums.push_back(enums_value::kTop);
  enums.push_back(enums_value::kMiddle);
  enums.push_back(enums_value::kBottom);
  return enums;
}

Validator::ValuesEnum Validator::PrepareMassageSeatAction() {
  ValuesEnum enums;
  enums.push_back(enums_value::kStart);
  enums.push_back(enums_value::kStop);
  return enums;
}

Validator::ValuesEnum Validator::PrepareMassageSeatZone() {
  ValuesEnum enums;
  enums.push_back(enums_value::kLumbar);
  enums.push_back(enums_value::kBottom);
  return enums;
}

Validator::ValuesEnum Validator::PrepareMassageSeatLevel() {
  ValuesEnum enums;
  enums.push_back(enums_value::kHigh);
  enums.push_back(enums_value::kLow);
  return enums;
}

Validator::ValuesEnum Validator::PrepareAudioSource() {
  ValuesEnum enums;
  enums.push_back(enums_value::kCD);
  enums.push_back(enums_value::kBluetooth);
  enums.push_back(enums_value::kTuner);
  return enums;
}


ValidationResult Validator::ValidateEnumValue(const std::string& value,
                                            ValidationScope& validation_scope) {
  int type = validation_scope[ValidationParams::ENUM_TYPE];
  ValuesEnumMap::const_iterator i = values_.find(type);
  if (i == values_.end()) {
    LOG4CXX_ERROR(logger_, "Unknow enum: " << type);
    return ValidationResult::INVALID_DATA;
  }

  const ValuesEnum& enums = i->second;
  bool found = std::find(enums.begin(), enums.end(), value) != enums.end();
  if (!found) {
    LOG4CXX_ERROR(logger_, "Wrong value for enum: " << type);
    return ValidationResult::INVALID_DATA;
  }

  return ValidationResult::SUCCESS;
}

ValidationResult Validator::ValidateArrray(const Json::Value& json,
                                           const ArrayWithStructureScope& scope,
                                           Json::Value& outgoing_json) {
  if (!json.isArray()) {
    LOG4CXX_INFO(logger_, scope.array_name <<" " <<" must be array!.");
    return ValidationResult::INVALID_DATA;
  }

  int array_size = json.size();

  if ((array_size < scope.min_size) || (array_size > scope.max_size)) {
    LOG4CXX_INFO(logger_, scope.array_name <<" " <<" out of scope!.");
    return ValidationResult::INVALID_DATA;
  }

  ValidationResult result = ValidationResult::SUCCESS;

  for (int i = 0; i < array_size; ++i) {
    result = scope.validator->Validate(
        json[i], outgoing_json[i]);

    if (result != ValidationResult::SUCCESS) {
      return result;
    }
  }

  return ValidationResult::SUCCESS;
}

}  // namespace validators

}  // namespace can_cooperation

