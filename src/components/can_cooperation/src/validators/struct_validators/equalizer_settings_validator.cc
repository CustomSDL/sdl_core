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

#include "can_cooperation/validators/struct_validators/equalizer_settings_validator.h"
#include "can_cooperation/can_module_constants.h"

namespace can_cooperation {

namespace validators {

CREATE_LOGGERPTR_GLOBAL(logger_, "EqualizerSettingsValidator")

using message_params::kChannel;
using message_params::kChannelSettings;

EqualizerSettingsValidator::EqualizerSettingsValidator() {
  // name="channel"
  channel_[ValidationParams::TYPE] = ValueType::STRING;
  channel_[ValidationParams::MIN_LENGTH] = 0;
  channel_[ValidationParams::MAX_LENGTH] = 500;
  channel_[ValidationParams::ARRAY] = 0;
  channel_[ValidationParams::MANDATORY] = 1;

  // name="channelSettings"
  channelSettings_[ValidationParams::TYPE] = ValueType::INT;
  channelSettings_[ValidationParams::MIN_VALUE] = 0;
  channelSettings_[ValidationParams::MAX_VALUE] = 100;
  channelSettings_[ValidationParams::ARRAY] = 0;
  channelSettings_[ValidationParams::MANDATORY] = 1;

  validation_scope_map_[kChannel] = &channel_;
  validation_scope_map_[kChannelSettings] = &channelSettings_;
}

ValidationResult EqualizerSettingsValidator::Validate(const Json::Value& json,
                                                 Json::Value& outgoing_json) {
  LOG4CXX_AUTO_TRACE(logger_);

  if (!json.isObject()) {
    LOG4CXX_ERROR(logger_, "EqualizerSettings must be struct");
    return INVALID_DATA;
  }

  return ValidateSimpleValues(json, outgoing_json);
}

}  // namespace validators

}  // namespace can_cooperation

