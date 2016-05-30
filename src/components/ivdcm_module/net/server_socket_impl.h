#ifndef SRC_COMPONENTS_IVDCM_MODULE_NET_SERVER_SOCKET_IMPL_H_
#define SRC_COMPONENTS_IVDCM_MODULE_NET_SERVER_SOCKET_IMPL_H_

#include <net/server_socket.h>

namespace net {

/**
 * Generic server socket operations implementation
 */
class ServerSocketImpl: public virtual ServerSocket {
 public:
  /**
   * Default constructor for AF_INET domain socket
   *
   * @param family  Specifies socket communication domain
   * @param address Specifies socket address.
   * @param port    Specifies socket port.
   */
  ServerSocketImpl(const char* address, UInt32 port);

  /**
   * Default constructor for AF_UNIX domain sockets
   *
   * @param path_name   Specifies socket communication domain
   */
  explicit ServerSocketImpl(const char* path_name);

  /**
   * Default Destructor
   */
  virtual ~ServerSocketImpl();

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
  virtual bool bind();

  /**
   * Puts the given socket in listen mode, so that it can accept
   * incoming connection requests.
   *
   * @param backlog How many connection requests should be queued,
   * defaults to 1
   */
  virtual bool listen(Int32 backlog = 1);

  /**
   * Accepts connection on the socket
   * @return descriptor for the accepted socket. In case of fail INVALID_SOCKET will be returned
   *
   */
  virtual ConnectedSocket* accept();
};

}  // namespace net

#endif  // SRC_COMPONENTS_IVDCM_MODULE_NET_SERVER_SOCKET_IMPL_H_
