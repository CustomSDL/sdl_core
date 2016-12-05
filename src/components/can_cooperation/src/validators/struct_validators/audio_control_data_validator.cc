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

#include "can_cooperation/validators/struct_validators/audio_control_data_validator.h"
#include "can_cooperation/validators/struct_validators/equalizer_settings_validator.h"
#include "can_cooperation/message_helper.h"
#include "can_cooperation/can_module_constants.h"

namespace can_cooperation {

namespace validators {

CREATE_LOGGERPTR_GLOBAL(logger_, "AudioControlDataValidator")

using message_params::kSource;
using message_params::kAudioVolume;
using message_params::kEqualizerSettings;

AudioControlDataValidator::AudioControlDataValidator() {

  // name="source"
  source_[ValidationParams::TYPE] = ValueType::ENUM;
  source_[ValidationParams::ENUM_TYPE] = EnumType::AUDIO_SOURCE;
  source_[ValidationParams::ARRAY] = 0;
  source_[ValidationParams::MANDATORY] = 0;

  // name="audioVolume"
  audio_volume_[ValidationParams::TYPE] = ValueType::INT;
  audio_volume_[ValidationParams::MIN_VALUE] = 0;
  audio_volume_[ValidationParams::MAX_VALUE] = 100;
  audio_volume_[ValidationParams::ARRAY] = 0;
  audio_volume_[ValidationParams::MANDATORY] = 0;

  validation_scope_map_[kSource] = &source_;
  validation_scope_map_[kAudioVolume] = &audio_volume_;
}

ValidationResult AudioControlDataValidator::Validate(const Json::Value& json,
                                                 Json::Value& outgoing_json) {
  LOG4CXX_AUTO_TRACE(logger_);

  if (!json.isObject()) {
    LOG4CXX_ERROR(logger_, "AudioControlData must be struct");
    return INVALID_DATA;
  }

  ValidationResult result = ValidateSimpleValues(json, outgoing_json);

  if (result != ValidationResult::SUCCESS) {
    return result;
  }

  const int array_min_size = 1;
  const int array_max_size = 10;

  // TODO(VS): create main function for array with struct validation
  if (IsMember(json, kEqualizerSettings)) {
    int array_size = json[kEqualizerSettings].size();
    if (json[kEqualizerSettings].isArray() && (array_size >= array_min_size)
        && (array_size <= array_max_size)) {
      for (int i = 0; i < array_size; ++i) {
        result = validators::EqualizerSettingsValidator::instance()->Validate(
            json[kEqualizerSettings][i], outgoing_json[kEqualizerSettings][i]);

        if (result != ValidationResult::SUCCESS) {
          return result;
        }
      }
    } else {
      LOG4CXX_ERROR(logger_, "EqualizerSettings wrong data!");
      return INVALID_DATA;
    }
  }

  if (!outgoing_json.size()) {
    result = INVALID_DATA;
  }

  return result;
}

}  // namespace validators

}  // namespace can_cooperation

