#include "net/server_socket.h"
#include "utils/logger.h"

CREATE_LOGGERPTR_GLOBAL(logger_, "IVDCM")

namespace net {

ServerSocket::ServerSocket(Domain domain, const char* address, UInt32 port)
  : Socket(domain, address, port) {
}

ServerSocket::~ServerSocket() {
  LOG4CXX_AUTO_TRACE(logger_);
  shutdown();
  close();
}

bool ServerSocket::set_opt(Int32 level, Int32 optname,
                     const void* optval, socklen_t opt_len) {
  return Socket::set_opt(level, optname, optval, opt_len);
}

void ServerSocket::close() {
  Socket::close();
}

void ServerSocket::shutdown() {
  Socket::shutdown();
}

void ServerSocket::set_blocking_mode(bool is_blocking) {
  Socket::set_blocking_mode(is_blocking);
}

}  // namespace net
