#ifndef SRC_COMPONENTS_IVDCM_MODULE_NET_SERVER_SOCKET_H_
#define SRC_COMPONENTS_IVDCM_MODULE_NET_SERVER_SOCKET_H_

#include <net/socket.h>

namespace net {

class ConnectedSocket;

/**
 * Generic server socket operations abstraction
 */
class ServerSocket: public Socket {
 public:
  /**
   * Default constructor
   *
   * @param family  Specifies socket communication domain
   * @param address Specifies socket address. In case of NET_AF_UNIX specifies pathname
   * @param port    Specifies socket port. In case of NET_AF_UNIX will be omitted
   */
  ServerSocket(Domain domain, const char* address, UInt32 port);

  /**
   * Default Destructor
   */
  virtual ~ServerSocket();

  /**
   * Sets options on socket
   * Wrapper around the setsockopt system call.
   */
  virtual bool set_opt(Int32 level, Int32 optname,
               const void* optval, socklen_t optlen);

  /**
   * Closes socket
   */
  virtual void close();

  /**
   * Shudowns socket using SHUT_RDWR option
   */
  virtual void shutdown();

  /**
   * Sets the blocking mode of this socket. If set to nonblocking,
   * read/write calls will always return immediately, but possibly
   * not all of the data has been read/written.
   *
   * @param is_blocking Whether to operate in blocking mode or not.
   */
  virtual void set_blocking_mode(bool is_blocking);

  /**
   * Binds the socket to the specified address.
   */
  virtual bool bind() = 0;

  /**
   * Puts the given socket in listen mode, so that it can accept
   * incoming connection requests.
   *
   * @param backlog How many connection requests should be queued
   *
   */
  virtual bool listen(Int32 backlog) = 0;

  /**
   * Accepts connection on the socket
   * @return descriptor for the accepted socket. In case of fail INVALID_SOCKET will be returned
   *
   */
  virtual ConnectedSocket* accept() = 0;
};

}  // namespace net

#endif  // SRC_COMPONENTS_IVDCM_MODULE_NET_SERVER_SOCKET_H_
