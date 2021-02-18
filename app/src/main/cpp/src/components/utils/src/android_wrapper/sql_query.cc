/*
 * Copyright (c) 2013, Ford Motor Company
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

#include "utils/android_wrapper/sql_query.h"
#include "utils/android_wrapper/sql_database.h"

namespace utils {
namespace dbms {

SQLQuery::SQLQuery(SQLDatabase* db)
    : db_(*db), query_(""), statement_(NULL), error_(0) {}

SQLQuery::~SQLQuery() {
  Finalize();
}

bool SQLQuery::Prepare(const std::string& query) {
  return true;
}

bool SQLQuery::Exec() {
  return true;
}

bool SQLQuery::Next() {
  return true;
}

bool SQLQuery::Reset() {
  return true;
}

void SQLQuery::Finalize() {
}

bool SQLQuery::Exec(const std::string& query) {
  return true;
}

void SQLQuery::Bind(int pos, int value) {
}

void SQLQuery::Bind(int pos, int64_t value) {
}

void SQLQuery::Bind(int pos, double value) {
}

void SQLQuery::Bind(int pos, bool value) {
}

void SQLQuery::Bind(int pos, const std::string& value) {
}

bool SQLQuery::GetBoolean(int pos) const {
  return true;
}

int SQLQuery::GetInteger(int pos) const {
  return 0;
}

uint32_t SQLQuery::GetUInteger(int pos) const {
  return 0;
}

int64_t SQLQuery::GetLongInt(int pos) const {
  return 0;
}

double SQLQuery::GetDouble(int pos) const {
  return 0;
}

std::string SQLQuery::GetString(int pos) const {
  return 0;
}

const std::string& SQLQuery::query() const {
  // TODO(KKolodiy): may return string query with value of arguments
  return query_;
}

bool SQLQuery::IsNull(int pos) const {
  return false;
}

void SQLQuery::Bind(int pos) {
}

SQLError SQLQuery::LastError() const {
  return SQLError(Error(error_));
}

int64_t SQLQuery::LastInsertId() const {
  return 0;
}

}  // namespace dbms
}  // namespace utils
