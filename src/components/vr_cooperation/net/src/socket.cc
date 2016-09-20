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

#include "net/socket.h"

#include <fcntl.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/un.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>

#include "utils/logger.h"

CREATE_LOGGERPTR_GLOBAL(logger_, "VRModule")

namespace net {

Socket::Socket(Domain domain, const char* address, UInt32 port)
    : domain_(domain),
      address_(address),
      port_number_(port),
      socket_(INVALID_SOCKET) {
  create_socket();
}

Socket::Socket(Domain domain, Int32 handle)
    : domain_(domain), address_(), port_number_(), socket_(handle) {}

Socket::~Socket() {}

bool Socket::set_opt(Int32 level, Int32 optname, const void* optval,
                     socklen_t opt_len) {
  if (-1 == setsockopt(socket_, level, optname,
                       static_cast<const void*>(&optval),
                       static_cast<socklen_t>(sizeof opt_len))) {
    LOG4CXX_FATAL(logger_, "Unable to set sockopt " << strerror(errno));
    return false;
  }

  return true;
}

void Socket::close() {
  if (INVALID_SOCKET == socket_) {
    LOG4CXX_WARN(logger_, "Trying to close invalid socket");
    return;
  }

  if (-1 == ::close(socket_)) {
    LOG4CXX_ERROR(logger_, "Unable to close socket " << strerror(errno));
    return;
  }
  socket_ = INVALID_SOCKET;
}

void Socket::shutdown() {
  if (INVALID_SOCKET == socket_) {
    LOG4CXX_WARN(logger_, "Trying to close invalid socket");
    return;
  }

  if (-1 == ::shutdown(socket_, SHUT_RDWR)) {
    LOG4CXX_ERROR(logger_, "Unable to shutdown socket " << strerror(errno));
    return;
  }
}

void Socket::set_blocking_mode(bool is_blocking) {
  if (is_blocking) {
    ::fcntl(socket_, F_SETFL, ::fcntl(socket_, F_GETFL, 0) & ~O_NDELAY);
  } else {
    ::fcntl(socket_, F_SETFL, ::fcntl(socket_, F_GETFL, 0) | O_NDELAY);
  }
}

void Socket::create_socket() {
  // socket type is SOCK_STREAM by default
  socket_ = socket(domain_, SOCK_STREAM, 0);
  if (-1 == socket_) {
    LOG4CXX_FATAL(logger_, "Unable to create socket " << strerror(errno));
  }
}

}  // namespace net
