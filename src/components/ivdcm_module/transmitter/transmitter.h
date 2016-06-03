#ifndef SRC_COMPONENTS_IVDCM_MODULE_TRANSMITTER_TRANSMITTER_H_
#define SRC_COMPONENTS_IVDCM_MODULE_TRANSMITTER_TRANSMITTER_H_
#include <string>
#include "utils/types.h"
#include "utils/macro.h"
#include "utils/logger.h"

CREATE_LOGGERPTR_GLOBAL(logger_, "IVDCM")

namespace net {
class ConnectedSocket;
}

namespace transmitter {
class Transmitter {
 public:
  explicit Transmitter(net::ConnectedSocket* socket = NULL)
      : socket_(socket), stop_flag_(false) {}
  bool Send(const std::string& to_send);
  bool Recv(std::string* message);

  template <class NotificationSubscriber>
  bool MessageListenLoop(NotificationSubscriber* subscriber) {
    while (!stop_flag_) {
      std::string buff;
      if (!Recv(&buff)) {
        LOG4CXX_ERROR(logger_, "Socket peer is down");
        return false;
      }
      if (buff.size() > 0) {
        subscriber->OnMessageReceived(buff);
      } else {
        LOG4CXX_WARN(logger_, "Received 0 bytes");
      }
    }
    return true;
  }
  void Stop();

 public:
  void set_socket(net::ConnectedSocket* socket) {
    socket_ = socket;
  }

 private:
  Int32 RecvSize();
  bool SendSize(const std::string& str);
  net::ConnectedSocket* socket_;
  volatile bool stop_flag_;
};

}  // namespace transmitter
#endif  // SRC_COMPONENTS_IVDCM_MODULE_TRANSMITTER_TRANSMITTER_H_
