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

TEST(ApplicationParamsValidationTest, ValidateModuleTypes) {
  ApplicationParams empty;
  EXPECT_TRUE(empty.ValidateModuleTypes());
  EXPECT_FALSE(empty.moduleType.is_initialized());

  ApplicationParams valid;
  ModuleTypes modules;
  modules.push_back(ModuleType::MT_AUDIO);
  modules.push_back(ModuleType::MT_SEATS);
  *valid.moduleType = modules;
  EXPECT_TRUE(valid.ValidateModuleTypes());
  EXPECT_TRUE(valid.moduleType.is_initialized());
  EXPECT_EQ(ModuleType::MT_AUDIO, valid.moduleType->at(0));
  EXPECT_EQ(ModuleType::MT_SEATS, valid.moduleType->at(1));

  ApplicationParams wrong_at_all;
  Json::Value value(Json::arrayValue);
  value.append(Json::Value("SEATS"));
  value.append(Json::Value("SA-21"));
  value.append(Json::Value("RADIO"));
  value.append(Json::Value("Improved Kilo"));
  ModuleTypes wrong_modules(&value);
  *wrong_at_all.moduleType = wrong_modules;
  EXPECT_TRUE(wrong_at_all.ValidateModuleTypes());
  EXPECT_TRUE(wrong_at_all.moduleType.is_initialized());
  EXPECT_EQ(2u, wrong_at_all.moduleType->size());
  EXPECT_EQ(ModuleType::MT_SEATS, wrong_at_all.moduleType->at(0));
  EXPECT_EQ(ModuleType::MT_RADIO, wrong_at_all.moduleType->at(1));
}

}  // namespace policy_table_interface_base
}  // namespace rpc
