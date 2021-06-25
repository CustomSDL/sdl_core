/*
 * \file bluetooth_le_device_scanner.cc
 * \brief BluetoothLeDeviceScanner class header file.
 *
 * Copyright (c) 2021, Ford Motor Company
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

#include "transport_manager/bluetooth_le/bluetooth_le_device_scanner.h"

#include <errno.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sstream>
#include <vector>

#include "transport_manager/bluetooth_le/bluetooth_le_device.h"
#include "transport_manager/bluetooth_le/bluetooth_le_transport_adapter.h"

#include "utils/logger.h"
#include "utils/threads/thread.h"

namespace transport_manager {
namespace transport_adapter {

SDL_CREATE_LOG_VARIABLE("TransportManager")

BluetoothLeDeviceScanner::BluetoothLeDeviceScanner(
    TransportAdapterController* controller,
    bool auto_repeat_search,
    int auto_repeat_pause_sec)
    : controller_(controller)
    , thread_(NULL)
    , shutdown_requested_(false)
    , ready_(true)
    , device_scan_requested_(false)
    , device_scan_requested_lock_()
    , device_scan_requested_cv_()
    , auto_repeat_search_(auto_repeat_search)
    , auto_repeat_pause_sec_(auto_repeat_pause_sec) {
  uint8_t smart_device_link_service_uuid_data[] = {0x93,
                                                   0x6D,
                                                   0xA0,
                                                   0x1F,
                                                   0x9A,
                                                   0xBD,
                                                   0x4D,
                                                   0x9D,
                                                   0x80,
                                                   0xC7,
                                                   0x02,
                                                   0xAF,
                                                   0x85,
                                                   0xC8,
                                                   0x22,
                                                   0xA8};
  thread_ = threads::CreateThread("BLE Device Scanner",
                                  new BLEDeviceScannerDelegate(this));
}

BluetoothLeDeviceScanner::BluetoothLeDeviceScanner(
    TransportAdapterController* controller,
    bool auto_repeat_search,
    int auto_repeat_pause_sec,
    const uint8_t* smart_device_link_service_uuid_data)
    : controller_(controller)
    , thread_(NULL)
    , shutdown_requested_(false)
    , ready_(true)
    , device_scan_requested_(false)
    , device_scan_requested_lock_()
    , device_scan_requested_cv_()
    , auto_repeat_search_(auto_repeat_search)
    , auto_repeat_pause_sec_(auto_repeat_pause_sec) {
  thread_ = threads::CreateThread("BT Device Scanner",
                                  new BLEDeviceScannerDelegate(this));
}

BluetoothLeDeviceScanner::~BluetoothLeDeviceScanner() {
  thread_->Stop(threads::Thread::kThreadSoftStop);
  delete thread_->GetDelegate();
  threads::DeleteThread(thread_);
}

bool BluetoothLeDeviceScanner::IsInitialised() const {
  return thread_ && thread_->IsRunning();
}

void BluetoothLeDeviceScanner::UpdateTotalDeviceList() {
  SDL_LOG_AUTO_TRACE();
  DeviceVector devices;
  devices.insert(devices.end(),
                 paired_devices_with_sdl_.begin(),
                 paired_devices_with_sdl_.end());
  devices.insert(devices.end(),
                 found_devices_with_sdl_.begin(),
                 found_devices_with_sdl_.end());
  controller_->SearchDeviceDone(devices);
}

void BluetoothLeDeviceScanner::DoInquiry() {
  SDL_LOG_AUTO_TRACE();

  // TODO: implement
}

void BluetoothLeDeviceScanner::CheckSDLServiceOnDevices(
    const std::vector<bdaddr_t>& bd_addresses,
    int device_handle,
    DeviceVector* discovered_devices) {
  SDL_LOG_TRACE("enter. bd_addresses: "
                << &bd_addresses << ", device_handle: " << device_handle
                << ", discovered_devices: " << discovered_devices);
  // TODO: implement
}

void BluetoothLeDeviceScanner::Thread() {
  SDL_LOG_AUTO_TRACE();
  ready_ = true;
  if (auto_repeat_search_) {
    while (!shutdown_requested_) {
      DoInquiry();
      device_scan_requested_ = false;
      TimedWaitForDeviceScanRequest();
    }
  } else {  // search only on demand
    while (true) {
      {
        sync_primitives::AutoLock auto_lock(device_scan_requested_lock_);
        while (!(device_scan_requested_ || shutdown_requested_)) {
          device_scan_requested_cv_.Wait(auto_lock);
        }
      }
      if (shutdown_requested_) {
        break;
      }
      DoInquiry();
      device_scan_requested_ = false;
    }
  }
}

void BluetoothLeDeviceScanner::TimedWaitForDeviceScanRequest() {
  SDL_LOG_AUTO_TRACE();

  if (auto_repeat_pause_sec_ == 0) {
    SDL_LOG_TRACE("exit. Condition: auto_repeat_pause_sec_ == 0");
    return;
  }

  {
    sync_primitives::AutoLock auto_lock(device_scan_requested_lock_);
    while (!(device_scan_requested_ || shutdown_requested_)) {
      const sync_primitives::ConditionalVariable::WaitStatus wait_status =
          device_scan_requested_cv_.WaitFor(auto_lock,
                                            auto_repeat_pause_sec_ * 1000);
      if (wait_status == sync_primitives::ConditionalVariable::kTimeout) {
        SDL_LOG_INFO("Bluetooth LE scanner timeout, performing scan");
        device_scan_requested_ = true;
      }
    }
  }
}

TransportAdapter::Error BluetoothLeDeviceScanner::Init() {
  SDL_LOG_AUTO_TRACE();
  /*
  if (!thread_->Start()) {
    SDL_LOG_ERROR("Bluetooth LE device scanner thread start failed");
    return TransportAdapter::FAIL;
  }
  SDL_LOG_INFO("Bluetooth LE device scanner thread started");
  */
  return TransportAdapter::OK;
}

void BluetoothLeDeviceScanner::Terminate() {
  SDL_LOG_AUTO_TRACE();
  shutdown_requested_ = true;
  if (thread_) {
    {
      sync_primitives::AutoLock auto_lock(device_scan_requested_lock_);
      device_scan_requested_ = false;
      device_scan_requested_cv_.NotifyOne();
    }
    SDL_LOG_INFO("Waiting for bluetooth le device scanner thread termination");
    thread_->Stop(threads::Thread::kThreadStopDelegate);
    SDL_LOG_INFO("Bluetooth LE device scanner thread stopped");
  }
}

TransportAdapter::Error BluetoothLeDeviceScanner::Scan() {
  SDL_LOG_AUTO_TRACE();
  if ((!IsInitialised()) || shutdown_requested_) {
    SDL_LOG_WARN("BAD_STATE");
    return TransportAdapter::BAD_STATE;
  }
  if (auto_repeat_pause_sec_ == 0) {
    return TransportAdapter::OK;
  }
  TransportAdapter::Error ret = TransportAdapter::OK;

  sync_primitives::AutoLock auto_lock(device_scan_requested_lock_);
  if (!device_scan_requested_) {
    SDL_LOG_TRACE("Requesting device Scan");
    device_scan_requested_ = true;
    device_scan_requested_cv_.NotifyOne();
  } else {
    ret = TransportAdapter::BAD_STATE;
    SDL_LOG_WARN("BAD_STATE");
  }
  return ret;
}

BluetoothLeDeviceScanner::BLEDeviceScannerDelegate::
    BLEDeviceScannerDelegate(BluetoothLeDeviceScanner* scanner)
    : scanner_(scanner) {}

void BluetoothLeDeviceScanner::BLEDeviceScannerDelegate::threadMain() {
  SDL_LOG_AUTO_TRACE();
  DCHECK(scanner_);
  scanner_->Thread();
}

}  // namespace transport_adapter
}  // namespace transport_manager
