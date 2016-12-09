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

TEST(ModuleTypesValidationTest, Validate) {
  ModuleTypesValidator validator;
  Optional<ModuleTypes> empty;
  EXPECT_TRUE(validator.Validate(empty));
  EXPECT_FALSE(empty.is_initialized());

  Optional<ModuleTypes> valid;
  valid->push_back(ModuleType::MT_AUDIO);
  valid->push_back(ModuleType::MT_SEATS);
  EXPECT_TRUE(validator.Validate(valid));
  EXPECT_TRUE(valid.is_initialized());
  EXPECT_EQ(ModuleType::MT_AUDIO, valid->at(0));
  EXPECT_EQ(ModuleType::MT_SEATS, valid->at(1));

  Json::Value value1(Json::arrayValue);
  value1.append(Json::Value("SEATS"));
  value1.append(Json::Value("SA-21"));
  value1.append(Json::Value("RADIO"));
  value1.append(Json::Value("Improved Kilo"));
  Optional<ModuleTypes> few_wrong_modules(&value1);
  EXPECT_TRUE(validator.Validate(few_wrong_modules));
  EXPECT_TRUE(few_wrong_modules->is_initialized());
  EXPECT_EQ(2u, few_wrong_modules->size());
  EXPECT_EQ(ModuleType::MT_SEATS, few_wrong_modules->at(0));
  EXPECT_EQ(ModuleType::MT_RADIO, few_wrong_modules->at(1));

  Json::Value value2(Json::arrayValue);
  value2.append(Json::Value("SS-N-27"));
  value2.append(Json::Value("Satan-2"));
  Optional<ModuleTypes> wrong_modules_at_all(&value2);
  EXPECT_TRUE(validator.Validate(wrong_modules_at_all));
  EXPECT_FALSE(wrong_modules_at_all->is_initialized());
  EXPECT_TRUE(wrong_modules_at_all->empty());
}

}  // namespace policy_table_interface_base
}  // namespace rpc
