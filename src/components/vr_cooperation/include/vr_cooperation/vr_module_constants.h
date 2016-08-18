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

#ifndef SRC_COMPONENTS_VR_COOPERATION_INCLUDE_VR_COOPERATION_VR_MODULE_CONSTANTS_H_
#define SRC_COMPONENTS_VR_COOPERATION_INCLUDE_VR_COOPERATION_VR_MODULE_CONSTANTS_H_

namespace vr_cooperation {

namespace result_codes {
const char kSuccess[] = "SUCCESS";
const char kUnsupportedRequest[] = "UNSUPPORTED_REQUEST";
const char kUnsupportedResource[] = "UNSUPPORTED_RESOURCE";
const char kDisallowed[] = "DISALLOWED";
const char kRejected[] = "REJECTED";
const char kAborted[] = "ABORTED";
const char kIgnored[] = "IGNORED";
const char kRetry[] = "RETRY";
const char kInUse[] = "IN_USE";
const char kVehicleDataNotAvailable[] = "VEHICLE_DATA_NOT_AVAILABLE";
const char kTimedOut[] = "TIMED_OUT";
const char kInvalidData[] = "INVALID_DATA";
const char kCharLimitExceeded[] = "CHAR_LIMIT_EXCEEDED";
const char kInvalidId[] = "INVALID_ID";
const char kDuplicateName[] = "DUPLICATE_NAME";
const char kApplicationNotRegistered[] = "APPLICATION_NOT_REGISTERED";
const char kOutOfMemory[] = "OUT_OF_MEMORY";
const char kTooManyPendingRequests[] = "TOO_MANY_PENDING_REQUESTS";
const char kWarnings[] = "WARNINGS";
const char kWrongLanguage[] = "WRONG_LANGUAGE";
const char kGenericError[] = "GENERIC_ERROR";
const char kUserDisallowed[] = "USER_DISALLOWED";
const char kReadOnly[] = "READ_ONLY";
}  // namespace result_codes

namespace json_keys {
const char kParams[] = "params";
const char kSuccess[] = "success";
const char kResultCode[] = "resultCode";
const char kResult[] = "result";
const char kInfo[] = "info";
const char kId[] = "id";
const char kJsonrpc[] = "jsonrpc";
const char kMethod[] = "method";
const char kError[] = "error";
const char kMessage[] = "message";
const char kData[] = "data";
const char kAppId[] = "appID";
const char kCode[] = "code";
const char kService[] = "service";
const char kDefault[] = "default";
const char kType[] = "type";
}  // namespace json_keys

}  // namespace vr_cooperation

#endif // SRC_COMPONENTS_VR_COOPERATION_INCLUDE_VR_COOPERATION_VR_MODULE_CONSTANTS_H_
