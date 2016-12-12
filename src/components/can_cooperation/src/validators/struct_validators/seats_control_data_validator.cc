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

#include "can_cooperation/validators/struct_validators/seats_control_data_validator.h"
#include "can_cooperation/can_module_constants.h"

namespace can_cooperation {

namespace validators {

CREATE_LOGGERPTR_GLOBAL(logger_, "SeatsControlDataValidator")

using message_params::kCooledSeats;
using message_params::kCooledSeatLevel;
using message_params::kHeatedSeat;
using message_params::kSeatHorizontalPosition;
using message_params::kSeatVerticalPosition;
using message_params::kSeatAnglePosition;
using message_params::kBackTiltPosition;
using message_params::kMassageSeat;
using message_params::kMassageSeatZone;
using message_params::kMassageSeatLevel;
using message_params::kBackLumbarPosition;
using message_params::kMassageEnabled;

SeatsControlDataValidator::SeatsControlDataValidator() {
  // name="cooledSeats"
  coooled_seats_[ValidationParams::TYPE] = ValueType::BOOL;
  coooled_seats_[ValidationParams::ARRAY] = 0;
  coooled_seats_[ValidationParams::MANDATORY] = 0;

  // name="cooledSeatLevel"
  coooled_seat_level_[ValidationParams::TYPE] = ValueType::INT;
  coooled_seat_level_[ValidationParams::MIN_VALUE] = 0;
  coooled_seat_level_[ValidationParams::MAX_VALUE] = 100;
  coooled_seat_level_[ValidationParams::ARRAY] = 0;
  coooled_seat_level_[ValidationParams::MANDATORY] = 0;

  // name="heatedSeat"
  heated_seat_[ValidationParams::TYPE] = ValueType::BOOL;
  heated_seat_[ValidationParams::ARRAY] = 0;
  heated_seat_[ValidationParams::MANDATORY] = 0;

  // name="seatHorizontalPosition"
  seat_horizontal_position_[ValidationParams::TYPE] = ValueType::INT;
  seat_horizontal_position_[ValidationParams::MIN_VALUE] = 0;
  seat_horizontal_position_[ValidationParams::MAX_VALUE] = 100;
  seat_horizontal_position_[ValidationParams::ARRAY] = 0;
  seat_horizontal_position_[ValidationParams::MANDATORY] = 0;

  // name="seatVerticalPosition"
  seat_vertical_position_[ValidationParams::TYPE] = ValueType::INT;
  seat_vertical_position_[ValidationParams::MIN_VALUE] = 0;
  seat_vertical_position_[ValidationParams::MAX_VALUE] = 100;
  seat_vertical_position_[ValidationParams::ARRAY] = 0;
  seat_vertical_position_[ValidationParams::MANDATORY] = 0;

  // name="seatAnglePosition"
  seat_angle_position_[ValidationParams::TYPE] = ValueType::INT;
  seat_angle_position_[ValidationParams::MIN_VALUE] = 0;
  seat_angle_position_[ValidationParams::MAX_VALUE] = 100;
  seat_angle_position_[ValidationParams::ARRAY] = 0;
  seat_angle_position_[ValidationParams::MANDATORY] = 0;

  // name="backTiltPosition"
  back_tilt_position_[ValidationParams::TYPE] = ValueType::INT;
  back_tilt_position_[ValidationParams::MIN_VALUE] = 0;
  back_tilt_position_[ValidationParams::MAX_VALUE] = 100;
  back_tilt_position_[ValidationParams::ARRAY] = 0;
  back_tilt_position_[ValidationParams::MANDATORY] = 0;

  // name="massageSeat"
  massage_seat_[ValidationParams::TYPE] = ValueType::ENUM;
  massage_seat_[ValidationParams::ENUM_TYPE] = EnumType::MASSAGE_SEAT_ACTION;
  massage_seat_[ValidationParams::ARRAY] = 0;
  massage_seat_[ValidationParams::MANDATORY] = 0;

  // name="massageSeatZone"
  massage_seat_zone_[ValidationParams::TYPE] = ValueType::ENUM;
  massage_seat_zone_[ValidationParams::ENUM_TYPE] = EnumType::MASSAGE_SEAT_ZONE;
  massage_seat_zone_[ValidationParams::ARRAY] = 0;
  massage_seat_zone_[ValidationParams::MANDATORY] = 0;

  // name="massageSeatLevel"
  massage_seat_level_[ValidationParams::TYPE] = ValueType::ENUM;
  massage_seat_level_[ValidationParams::ENUM_TYPE] = EnumType::MASSAGE_SEAT_LEVEL;
  massage_seat_level_[ValidationParams::ARRAY] = 0;
  massage_seat_level_[ValidationParams::MANDATORY] = 0;

  // name="backLumbarPosition"
  back_lumbar_position_[ValidationParams::TYPE] = ValueType::ENUM;
  back_lumbar_position_[ValidationParams::ENUM_TYPE] = EnumType::LUMBAR_POSITION;
  back_lumbar_position_[ValidationParams::ARRAY] = 0;
  back_lumbar_position_[ValidationParams::MANDATORY] = 0;

  // name="massageEnabled"
  massage_enabled_[ValidationParams::TYPE] = ValueType::BOOL;
  massage_enabled_[ValidationParams::ARRAY] = 0;
  massage_enabled_[ValidationParams::MANDATORY] = 0;

  validation_scope_map_[kCooledSeats] = &coooled_seats_;
  validation_scope_map_[kCooledSeatLevel] = &coooled_seat_level_;
  validation_scope_map_[kHeatedSeat] = &heated_seat_;
  validation_scope_map_[kSeatHorizontalPosition] = &seat_horizontal_position_;
  validation_scope_map_[kSeatVerticalPosition] = &seat_vertical_position_;
  validation_scope_map_[kSeatAnglePosition] = &seat_angle_position_;
  validation_scope_map_[kBackTiltPosition] = &back_tilt_position_;
  validation_scope_map_[kMassageSeat] = &massage_seat_;
  validation_scope_map_[kMassageSeatZone] = &massage_seat_zone_;
  validation_scope_map_[kMassageSeatLevel] = &massage_seat_level_;
  validation_scope_map_[kBackLumbarPosition] = &back_lumbar_position_;
  validation_scope_map_[kMassageEnabled] = &massage_enabled_;
}

ValidationResult SeatsControlDataValidator::Validate(const Json::Value& json,
                                                 Json::Value& outgoing_json) {
  LOG4CXX_AUTO_TRACE(logger_);

  if (!json.isObject()) {
    LOG4CXX_ERROR(logger_, "SeatsControlData must be struct");
    return INVALID_DATA;
  }

  ValidationResult result = ValidateSimpleValues(json, outgoing_json);

  if (result != ValidationResult::SUCCESS) {
    return result;
  }

   if (!outgoing_json.size()) {
    result = INVALID_DATA;
  }

  return result;
}

void SeatsControlDataValidator::RemoveReadOnlyParams(Json::Value& json) {
  if (json.isMember(kHeatedSeat)) {
    json.removeMember(kHeatedSeat);
  }

  if (json.isMember(kCooledSeats)) {
    json.removeMember(kCooledSeats);
  }

  if (json.isMember(kMassageEnabled)) {
    json.removeMember(kMassageEnabled);
  }
}

}  // namespace validators

}  // namespace can_cooperation

