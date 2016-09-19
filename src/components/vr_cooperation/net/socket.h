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

#ifndef SRC_COMPONENTS_VR_COOPERATION_NET_SOCKET_H_
#define SRC_COMPONENTS_VR_COOPERATION_NET_SOCKET_H_

#include <sys/socket.h>
#include <sys/types.h>
#include <string>
#include "utils/types.h"
#include "utils/macro.h"

namespace net {

#define INVALID_SOCKET -1

/**
* Socket address family.
*/
enum Domain { NET_AF_UNIX = AF_UNIX, NET_AF_INET = AF_INET };

/**
* Socket address family.
*/
enum { NET_MSG_DONTWAIT = MSG_DONTWAIT, NET_MSG_NOSIGNAL = MSG_NOSIGNAL };

/**
 * Generic socket operations abstraction
 */
class Socket {
 public:
  /**
   * Default constructor
   *
   * @param family  Specifies socket communication domain
   * @param address Specifies socket address. In case of NET_AF_UNIX specifies
   *pathname
   * @param port    Specifies socket port. In case of NET_AF_UNIX will be
   *omitted
   */
  Socket(Domain domain, const char* address, UInt32 port);

  /**
   * Default Destructor
   */
  virtual ~Socket();

  /**
   * Sets options on socket
   * Wrapper around the setsockopt system call.
   */
  virtual bool set_opt(Int32 level, Int32 optname, const void* optval,
                       socklen_t optlen) = 0;

  /**
   * Closes socket
   */
  virtual void close() = 0;

  /**
   * Shudowns socket using SHUT_RDWR option
   */
  virtual void shutdown() = 0;

  /**
   * Sets the blocking mode of this socket. If set to nonblocking,
   * read/write calls will always return immediately, but possibly
   * not all of the data has been read/written.
   *
   * @param is_blocking Whether to operate in blocking mode or not.
   */
  virtual void set_blocking_mode(bool is_blocking) = 0;

  /**
   * Returns socket domain type
   */
  inline Domain domain() const;

  /**
   * Returns socket handle
   */
  inline Int32 socket_handle() const;

  /**
   * Returns socket port number
   */
  inline UInt32 port() const;

  /**
   * Returns socket address
   */
  const char* address() const;

  /**
   * Check whether the socket is blocking or not.
   *
   * @return true if the socket is in blocking mode.
   */
  bool is_blocking() const;

  /**
   * @brief Gets socket state.
   * @return TRUE if it is connected otherwise FALSE
   */
  inline bool is_connected() const;

  /**
   * @brief Checks socket file descriptor
   * @return TRUE if it is valid otherwise FALSE
   */
  bool is_valid() const;

 protected:
  Socket(Domain domain, Int32 handle);

  /**
   * @brief Creates a socket handle.
   */
  void create_socket();

 private:
  Domain domain_;
  std::string address_;
  Int32 port_number_;
  Int32 socket_;
};

inline Domain Socket::domain() const { return domain_; }

inline Int32 Socket::socket_handle() const { return socket_; }

inline UInt32 Socket::port() const { return port_number_; }

inline const char* Socket::address() const { return address_.c_str(); }

bool Socket::is_connected() const { return socket_ != INVALID_SOCKET; }

}  // namespace net

#endif  // SRC_COMPONENTS_VR_COOPERATION_NET_SOCKET_H_
