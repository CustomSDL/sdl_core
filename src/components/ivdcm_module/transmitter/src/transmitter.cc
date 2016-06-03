#include "transmitter/transmitter.h"

#include "net/connected_socket.h"
#include "utils/scope_guard.h"
#include "utils/macro.h"

namespace transmitter {
static const int HEADER_SIZE = sizeof(Int32);

Int32 Transmitter::RecvSize() {
  UInt8 result[HEADER_SIZE] = {0};
  ssize_t received_size = socket_->recv(result, HEADER_SIZE, 0);
  if (received_size < 0) {
    return received_size;
  }
  DCHECK_OR_RETURN(received_size == HEADER_SIZE, -1);
  const Int32 value = (Int32(result[0])) + (Int32(result[1]) << 8) +
                      (Int32(result[2]) << 16) + (Int32(result[3]) << 24);
  LOG4CXX_INFO(logger_, "Received size " << value);
  return value;
}

bool Transmitter::SendSize(const std::string& str) {
  DCHECK_OR_RETURN("Unable to send string with size = 0", false)
  UInt32 value = str.size();
  UInt8 result[HEADER_SIZE] = {0};
  result[0] = (value & 0x000000ff);
  result[1] = (value & 0x0000ff00) >> 8;
  result[2] = (value & 0x00ff0000) >> 16;
  result[3] = (value & 0xff000000) >> 24;
  LOG4CXX_INFO(logger_, "Send size " << value);
  return (socket_->send(result, HEADER_SIZE, 0) == 4);
}

bool Transmitter::Send(const std::string& to_send) {
  DCHECK_OR_RETURN(socket_, false);
  const size_t chunk = 1000;
  size_t const total_size = to_send.size();
  size_t size_to_send = 0;
  UInt8* data_bytes = NULL;

  if (to_send.size() < chunk) {
    data_bytes = new UInt8[to_send.size()];
    size_to_send = to_send.size();
  } else {
    data_bytes = new UInt8[chunk];
    size_to_send = chunk;
  }
  utils::ScopeGuard ppsdata_guard =
      utils::MakeGuard(utils::ArrayDeleter<UInt8*>, data_bytes);
  UNUSED(ppsdata_guard);

  SendSize(to_send);
  LOG4CXX_INFO(logger_, "Send data " << to_send.size());
  size_t data_sent = 0;
  while (total_size > data_sent) {
    memcpy(data_bytes, to_send.c_str() + data_sent, size_to_send);
    size_t chunksent =
        socket_->send(data_bytes, size_to_send, net::NET_MSG_NOSIGNAL);
    data_sent += chunksent;
    memset(data_bytes, 0, size_to_send);
    if ((data_sent + chunk) > total_size) {
      size_to_send = total_size - data_sent;
    }
  }
  LOG4CXX_INFO(logger_, "Sent " << data_sent);
  return true;
}

bool Transmitter::Recv(std::string* message) {
  DCHECK_OR_RETURN(socket_, false);
  const Int32 CHUNK_SIZE = 1000;
  Int32 to_recv = CHUNK_SIZE;
  Int32 wait_size = RecvSize();
  if (wait_size < 0) {
    LOG4CXX_ERROR(logger_, "Unable to receive message");
    return false;
  }
  LOG4CXX_ERROR(logger_, "Waiting for " << wait_size << " bytes");
  if (wait_size < to_recv) {
    to_recv = wait_size;
  }

  Int32 received = 0;
  UInt8* data_bytes = new UInt8[CHUNK_SIZE];

  while (wait_size > received) {
    ssize_t size = socket_->recv(data_bytes, to_recv, 0);
    if (size <= 0) {
      LOG4CXX_INFO(logger_, "recv error " << size);
      return false;
    }

    std::string partial;
    partial.assign(reinterpret_cast<char*>(data_bytes), size);
    message->append(partial);
    memset(data_bytes, 0, size);

    received += size;  // Just for debug
    if (received + CHUNK_SIZE > wait_size) {
      to_recv = wait_size - received;
    }
  }

  LOG4CXX_INFO(logger_, "Total received " << received);
  LOG4CXX_INFO(logger_, "String size " << message->size());
  return true;
}

void Transmitter::Stop() {
  stop_flag_ = true;
  if (socket_) {
    socket_->shutdown();
  }
}

}  // namespace transmitter
