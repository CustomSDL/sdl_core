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

#ifndef SRC_COMPONENTS_CAN_COOPERATION_INCLUDE_CAN_COOPERATION_VALIDATORS_STRUCT_VALIDATORS_SEATS_CONTROL_DATA_VALIDATOR_H_
#define SRC_COMPONENTS_CAN_COOPERATION_INCLUDE_CAN_COOPERATION_VALIDATORS_STRUCT_VALIDATORS_SEATS_CONTROL_DATA_VALIDATOR_H_

#include "can_cooperation/validators/validator.h"
#include "utils/singleton.h"

namespace can_cooperation {

namespace validators {

/**
 * @brief SeatsControlDataValidator class
 */
class SeatsControlDataValidator : public Validator, public utils::Singleton<SeatsControlDataValidator> {
 public:
  /**
   * @brief Validate json with message params
   *
   * @param json incoming json
   * @param outgoing_json outgoing json where is param will be copied after verification
   *
   * @return validation result
   */
  ValidationResult Validate(const Json::Value& json,
                            Json::Value& outgoing_json);

  /**
   * @brief Remove read only params from json
   *
   * @param json incoming json. Read only params will be cut off.
   */
  void RemoveReadOnlyParams(Json::Value& json);

 private:
  FRIEND_BASE_SINGLETON_CLASS(SeatsControlDataValidator);
  SeatsControlDataValidator();
  ValidationScope coooled_seats_;
  ValidationScope coooled_seat_level_;
  ValidationScope heated_seat_;
  ValidationScope seat_horizontal_position_;
  ValidationScope seat_vertical_position_;
  ValidationScope seat_angle_position_;
  ValidationScope back_tilt_position_;
  ValidationScope massage_seat_;
  ValidationScope massage_seat_zone_;
  ValidationScope massage_seat_level_;
  ValidationScope massage_enabled_;
  ValidationScope back_lumbar_position_;
  DISALLOW_COPY_AND_ASSIGN(SeatsControlDataValidator);
};

}  // namespace validators

}  // namespace can_cooperation

#endif  // SRC_COMPONENTS_CAN_COOPERATION_INCLUDE_CAN_COOPERATION_VALIDATORS_STRUCT_VALIDATORS_SEATS_CONTROL_DATA_VALIDATOR_H_
