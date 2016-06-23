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

#include "ivdcm_module/ip_data_sender.h"

#include "utils/logger.h"

namespace ivdcm_module {

CREATE_LOGGERPTR_GLOBAL(logger_, "IVDCM")

IpDataSender::IpDataSender()
    : tuns_(),
      to_tun_("IpDataSender", this),
      lock_() {
}

bool IpDataSender::AddTun(int id, const std::string& name) {
  LOG4CXX_AUTO_TRACE(logger_);
  char errbuf[PCAP_ERRBUF_SIZE];
  pcap_t *fd = pcap_open_live(name.c_str(), 65535, 0, 10, errbuf);
  if (fd) {
    sync_primitives::AutoLock locker(lock_);
    tuns_[id] = fd;
    return true;
  } else {
    LOG4CXX_ERROR(logger_, "Could not open TUN interface " << name);
    return false;
  }
}

void IpDataSender::RemoveTun(int id) {
  LOG4CXX_AUTO_TRACE(logger_);
  bool exist = tuns_.find(id) != tuns_.end();
  if (exist) {
    sync_primitives::AutoLock locker(lock_);
    pcap_close(tuns_[id]);
    tuns_.erase(id);
  } else {
    LOG4CXX_WARN(logger_, "TUN interface (" << id << ") is not exist");
  }
}

void IpDataSender::Send(int id, const std::vector<uint8_t>& data) {
  LOG4CXX_AUTO_TRACE(logger_);
  to_tun_.PostMessage(DataMessage(id, data));
}

void IpDataSender::Handle(const DataMessage message) {
  LOG4CXX_AUTO_TRACE(logger_);
  int id = message.first;
  pcap_t *fd = 0;
  lock_.Acquire();
  bool exist = tuns_.find(id) != tuns_.end();
  if (exist) {
    fd = tuns_[id];
  }
  lock_.Release();
  const std::vector<uint8_t>& data = message.second;
  int ret = pcap_inject(fd, &(data)[0], data.size());
  if (ret == -1) {
    std::string error(pcap_geterr(fd));
    LOG4CXX_ERROR(logger_, "Could not inject (" << id << "): " << error);
  }
}
}  // namespace ivdcm_module
