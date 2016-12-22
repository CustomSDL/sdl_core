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

#include "can_cooperation/validators/struct_validators/temperature_validator.h"
#include "can_cooperation/can_module_constants.h"
#include "can_cooperation/message_helper.h"

namespace can_cooperation {

namespace validators {

CREATE_LOGGERPTR_GLOBAL(logger_, "TemperatureValidator")

using message_params::kValueC;
using message_params::kValueF;
using message_params::kUnit;

TemperatureValidator::TemperatureValidator() {
  // name="ValueC"
  value_c_[ValidationParams::TYPE] = ValueType::DOUBLE;
  value_c_[ValidationParams::MIN_VALUE] = 14;
  value_c_[ValidationParams::MAX_VALUE] = 30;
  value_c_[ValidationParams::ARRAY] = 0;
  value_c_[ValidationParams::MANDATORY] = 0;

  // name="ValueF"
  value_f_[ValidationParams::TYPE] = ValueType::DOUBLE;
  value_f_[ValidationParams::MIN_VALUE] = 60;
  value_f_[ValidationParams::MAX_VALUE] = 90;
  value_f_[ValidationParams::ARRAY] = 0;
  value_f_[ValidationParams::MANDATORY] = 0;

  validation_scope_map_[kValueF] = &value_f_;
  validation_scope_map_[kValueC] = &value_c_;
}

ValidationResult TemperatureValidator::Validate(const Json::Value& json,
                                                Json::Value& outgoing_json) {
  LOG4CXX_AUTO_TRACE(logger_);

  Json::Value desiredJson;

  if (IsMember(json, kUnit)) {
    if (IsEqual(json[kUnit], enums_value::kFahrenheit)) {
      if (!IsMember(json, kValueF)) {
        LOG4CXX_ERROR(logger_, "Param " << kValueF << " is missed!");
        return INVALID_DATA;
      }

      desiredJson[kValueF] = json[kValueF];
    } else if (IsEqual(json[kUnit], enums_value::kCelsius)) {
      if (!IsMember(json, kValueC)) {
        LOG4CXX_ERROR(logger_, "Param " << kValueC << " is missed!");
        return INVALID_DATA;
      }

      desiredJson[kValueC] = json[kValueC];
    } else {
      LOG4CXX_ERROR(logger_, "Wrong TemperatureUnit enum value!");
      return ValidationResult::INVALID_DATA;
    }
  } else {
    LOG4CXX_ERROR(logger_, "Mandatory param " << kUnit << " is missed!");
    return INVALID_DATA;
  }

  outgoing_json[kUnit] = json[kUnit];

  return ValidateSimpleValues(desiredJson, outgoing_json);
}

}  // namespace validators

}  // namespace can_cooperation
