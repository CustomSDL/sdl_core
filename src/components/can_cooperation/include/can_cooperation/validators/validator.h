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

#ifndef SRC_COMPONENTS_CAN_COOPERATION_INCLUDE_CAN_COOPERATION_VALIDATORS_VALIDATOR_H_
#define SRC_COMPONENTS_CAN_COOPERATION_INCLUDE_CAN_COOPERATION_VALIDATORS_VALIDATOR_H_

#include <map>
#include <string>
#include <vector>

#include "json/json.h"
#include "utils/macro.h"

namespace can_cooperation {

namespace validators {

enum ValidationResult {
  SUCCESS,
  INVALID_DATA
};


// TOD(VS): Use this enum as key for ValidationScope map
enum ValidationParams {
  MIN_LENGTH,
  MAX_LENGTH,
  MIN_VALUE,
  MAX_VALUE,
  MIN_SIZE,
  MAX_SIZE,
  ARRAY,
  MANDATORY,
  TYPE,
  ENUM_TYPE
};


enum ValueType {
  INT,
  STRING,
  DOUBLE,
  BOOL,
  ENUM
};

enum EnumType {
  BUTTON_NAME,
  MODULE_TYPE,
  RADIO_BAND,
  RADIO_STATE,
  DEFROST_ZONE,
  BUTTON_PRESS_MODE,
  VENTILATION_MODE,
  DISPLAY_MODE,
  DISTANCE_UNIT,
  TEMPERATURE_UNIT,
  LUMBAR_POSITION,
  MASSAGE_SEAT_ACTION,
  MASSAGE_SEAT_ZONE,
  MASSAGE_SEAT_LEVEL,
  AUDIO_SOURCE
};

// validation_scope map with data that will be used for validation(minlength. maxlength, etc.)
typedef std::map<ValidationParams, int> ValidationScope;

typedef std::map<std::string, ValidationScope*> ValidationScopeMap;

/**
 * @brief Class for simple data types validation
 */
class Validator {
 public:
  virtual ~Validator() {}

  /**
   * @brief Validate json with message params
   *
   * @param json incoming json
   * @param outgoing_json outgoing json where is param will be copied after verification
   *
   * @return validation result
   */
  virtual ValidationResult Validate(const Json::Value& json,
                                    Json::Value& outgoing_json);

 protected:
  /**
   * @brief Validate simple values(integers, strings, boolean), cut fake params,
   *        If invalid param will be found, validation stops, error will be returned as result,
   *        and outgoing_json should not be used.
   *
   * @param json incoming json with request-response params
   * @param outgoing_json outgoing json where is param will be copied after
   *                 verification(without fake params)
   *
   * @return validation result
   */
  ValidationResult ValidateSimpleValues(const Json::Value& json,
                                        Json::Value& outgoing_json);

  struct ArrayWithStructureScope;

  /**
   * @brief Validate array with structures
   *
   * @param json incoming json with array
    * @param outgoing_json outgoing json where is param will be copied
   * @param scope validation params
   *
   * @return validation result
   */
  static ValidationResult ValidateArrray(const Json::Value& json,
                                         const ArrayWithStructureScope& scope,
                                         Json::Value& outgoing_json);

  struct ArrayWithStructureScope {
    Validator* validator;
    const char* array_name;
    int min_size;
    int max_size;
  };

  ValidationScopeMap validation_scope_map_;

 private:
  typedef std::vector<std::string> ValuesEnum;
  typedef std::map<int, ValuesEnum> ValuesEnumMap;
  static const ValuesEnumMap values_;

  static ValuesEnumMap PrepareEnumValidator();
  static ValuesEnum PrepareButtonName();
  static ValuesEnum PrepareModuleType();
  static ValuesEnum PrepareRadioBand();
  static ValuesEnum PrepareRadioState();
  static ValuesEnum PrepareDefrostZone();
  static ValuesEnum PrepareButtonPressMode();
  static ValuesEnum PrepareVentilationMode();
  static ValuesEnum PrepareDisplayMode();
  static ValuesEnum PrepareDistanceUnit();
  static ValuesEnum PrepareTemperatureUnit();
  static ValuesEnum PrepareLumbarPosition();
  static ValuesEnum PrepareMassageSeatAction();
  static ValuesEnum PrepareMassageSeatZone();
  static ValuesEnum PrepareMassageSeatLevel();
  static ValuesEnum PrepareAudioSource();

  /**
   * @brief Validate value
   *
   * @param value_name name of param in json, that will be verified
   * @param json incoming json with request-response params
   * @param outgoing_json outgoing json where is param will be copied after verification
   * @param validation_scope map with data that will be used for validation(minlength, maxlength, etc.)
   *
   * @return validation result
   */
  ValidationResult ValidateValue(const std::string& value_name,
                                 const Json::Value& json,
                                 Json::Value& outgoing_json,
                                 ValidationScope& validation_scope);

  /**
   * @brief Validate value
   *
   * @param json incoming json with request-response params
   * @param validation_scope map with data that will be used for validation(minlength, maxlength, etc.)
   *
   * @return validation result
   */
  ValidationResult Validate(const Json::Value& json,
                            ValidationScope& validation_scope);
  /**
   * @brief Validate string value
   *
   * @param value string to verify
   * @param validation_scope map with data that will be used for validation(minlength, maxlength)
   *
   * @return validation result
   */
  ValidationResult ValidateStringValue(const std::string& value,
                                       ValidationScope& validation_scope);

  /**
   * @brief Validate string with enum value that comes in request/response/notification
   *
   * @param value value to verify
   * @param validation_scope map with data that will be used for validation
   *
   * @return true if value right, otherwise false
   */
  ValidationResult ValidateEnumValue(const std::string& value,
                                     ValidationScope& validation_scope);

  /**
   * @brief Validate int value
   *
   * @param value int value to verify
   * @param validation_scope map with data that will be used for validation(minvalue, maxvalue)
   *
   * @return validation result
   */
  ValidationResult ValidateIntValue(int value,
                                    ValidationScope& validation_scope);

  /**
   * @brief Validate double value
   *
   * @param value double value to verify
   * @param validation_scope map with data that will be used for validation(minvalue, maxvalue, etc.)
   *
   * @return validation result
   */
  ValidationResult ValidateDoubleValue(double value,
                                       ValidationScope& validation_scope);

  FRIEND_TEST(EnumValidatorTest, ValidateEnumValueSuccess);
  FRIEND_TEST(EnumValidatorTest, ValidateEnumValueInvalidData);
};

}  // namespace validators

}  // namespace can_cooperation

#endif  // SRC_COMPONENTS_CAN_COOPERATION_INCLUDE_CAN_COOPERATION_VALIDATORS_VALIDATOR_H_
