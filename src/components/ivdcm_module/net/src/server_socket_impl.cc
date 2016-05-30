#include "net/server_socket_impl.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <errno.h>

#include "net/connected_socket_impl.h"
#include "utils/logger.h"

CREATE_LOGGERPTR_GLOBAL(logger_, "IVDCM")

namespace net {

ServerSocketImpl::ServerSocketImpl(const char* address, UInt32 port)
    : ServerSocket(NET_AF_INET, address, port) {}

ServerSocketImpl::ServerSocketImpl(const char* path_name)
    : ServerSocket(NET_AF_UNIX, path_name, 0) {}

ServerSocketImpl::~ServerSocketImpl() {}

bool ServerSocketImpl::set_opt(Int32 level, Int32 optname, const void* optval,
                               socklen_t opt_len) {
  return ServerSocket::set_opt(level, optname, optval, opt_len);
}

void ServerSocketImpl::close() { ServerSocket::close(); }

void ServerSocketImpl::shutdown() { ServerSocket::shutdown(); }

void ServerSocketImpl::set_blocking_mode(bool is_blocking) {
  ServerSocket::set_blocking_mode(is_blocking);
}

bool ServerSocketImpl::bind() {
  struct sockaddr_in addr_in = {0};
  struct sockaddr_un addr_un = {0};
  struct sockaddr* addr = NULL;
  socklen_t len = 0;

  if (NET_AF_UNIX == domain()) {
    addr_un.sun_family = domain();
    memcpy(addr_un.sun_path, address(), sizeof(addr_un.sun_path));
    addr = reinterpret_cast<struct sockaddr*>(&addr_un);
    len = sizeof(addr_un);
  } else {
    addr_in.sin_addr.s_addr = inet_addr(address());
    addr_in.sin_family = domain();
    addr_in.sin_port = htons(port());
    addr = reinterpret_cast<struct sockaddr*>(&addr_in);
    len = sizeof(addr_in);
  }

  if (-1 == ::bind(socket_handle(), addr, len)) {
    std::string error(strerror(errno));
    LOG4CXX_FATAL(
        logger_,
        "Unable to bind to " <<  address() << ":" << port() << ". " << error);
    return false;
  }

  return true;
}

bool ServerSocketImpl::listen(Int32 backlog) {
  if (-1 == ::listen(socket_handle(), backlog)) {
    std::string error(strerror(errno));
    LOG4CXX_FATAL(
            logger_,
            "Unable to listen. " << error);
    return false;
  }

  return true;
}

ConnectedSocket* ServerSocketImpl::accept() {
  Int32 new_socket_fd = INVALID_SOCKET;
  ConnectedSocket* connected_socket = NULL;

  // The returned address is ignored
  LOG4CXX_INFO(logger_, "Accept connection on " << address() << ":" << port());
  new_socket_fd = ::accept(socket_handle(), NULL, NULL);
  if (0 > new_socket_fd) {
    std::string error(strerror(errno));
    LOG4CXX_ERROR(logger_, "Unable to accept " << error);
    return connected_socket;
  }

  connected_socket = new ConnectedSocketImpl(domain(), new_socket_fd);
  LOG4CXX_INFO(logger_, "Accepted new connection on " << address() << ":" << port());
  return connected_socket;
}

}  // namespace net
