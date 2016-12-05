/*
 * Copyright (c) 2016, Ford Motor Company
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following
 * disclaimer in the documentation and/or other materials provided with the
 * distribution.
 *
 * Neither the name of the Ford Motor Company nor the names of its contributors
 * may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <vector>

#include "gtest/gtest.h"
#include "types.h"
#include "validation.h"

namespace rpc {
namespace policy_table_interface_base {

TEST(AccessModulesValidationTest, Validate) {
  AccessModulesValidator validator;

  AccessModules empty_modules;
  EXPECT_TRUE(validator.Validate(empty_modules));

  AccessModules wrong_modules;
  wrong_modules["RADIO"] = RemoteRpcs();
  wrong_modules["SA-21"] = RemoteRpcs();
  EXPECT_FALSE(validator.Validate(wrong_modules));

  AccessModules true_modules;
  true_modules["RADIO"] = RemoteRpcs();
  true_modules["SEATS"] = RemoteRpcs();
  EXPECT_TRUE(validator.Validate(true_modules));

  AccessModules modules_with_rpcs;
  RemoteRpcs radio_rpcs;
  radio_rpcs["ButtonPress"] = Strings();
  modules_with_rpcs["RADIO"] = radio_rpcs;
  RemoteRpcs seats_rpcs;
  seats_rpcs["SetInteriorVehicleData"] = Strings();
  modules_with_rpcs["SEATS"] = seats_rpcs;
  EXPECT_TRUE(validator.Validate(modules_with_rpcs));

  AccessModules modules_with_wrong_rpcs;
  RemoteRpcs radio_wrong_rpcs;
  radio_wrong_rpcs["ButtonPress"] = Strings();
  modules_with_wrong_rpcs["RADIO"] = radio_wrong_rpcs;
  RemoteRpcs seats_wrong_rpcs;
  seats_wrong_rpcs["Fire"] = Strings();
  modules_with_wrong_rpcs["SEATS"] = seats_wrong_rpcs;
  EXPECT_FALSE(validator.Validate(modules_with_wrong_rpcs));

  AccessModules modules_with_params;
  RemoteRpcs radio_params;
  radio_params["ButtonPress"].push_back("band");
  radio_params["ButtonPress"].push_back("rdsData");
  modules_with_params["RADIO"] = radio_params;
  RemoteRpcs seats_params;
  seats_params["SetInteriorVehicleData"].push_back("heatedSeat");
  seats_params["SetInteriorVehicleData"].push_back("massageEnabled");
  modules_with_params["SEATS"] = seats_params;
  EXPECT_TRUE(validator.Validate(modules_with_params));

  AccessModules modules_with_wrong_params;
  RemoteRpcs radio_wrong_params;
  radio_wrong_params["ButtonPress"].push_back("band");
  radio_wrong_params["ButtonPress"].push_back("rdsData");
  modules_with_wrong_params["RADIO"] = radio_wrong_params;
  RemoteRpcs seats_wrong_params;
  seats_wrong_params["SetInteriorVehicleData"].push_back("heatedSeat");
  seats_wrong_params["SetInteriorVehicleData"].push_back("burn");
  modules_with_wrong_params["SEATS"] = seats_wrong_params;
  EXPECT_FALSE(validator.Validate(modules_with_wrong_params));

  AccessModules modules_with_mix_params;
  RemoteRpcs radio_mix_params;
  radio_mix_params["ButtonPress"].push_back("band");
  radio_mix_params["ButtonPress"].push_back("rdsData");
  modules_with_mix_params["RADIO"] = radio_mix_params;
  RemoteRpcs seats_mix_params;
  seats_mix_params["SetInteriorVehicleData"].push_back("hdChannel");
  seats_mix_params["SetInteriorVehicleData"].push_back("heatedSeat");
  modules_with_mix_params["SEATS"] = seats_mix_params;
  EXPECT_FALSE(validator.Validate(modules_with_mix_params));
}

}  // namespace policy_table_interface_base
}  // namespace rpc
