/*
 * \file bluetooth_le_transport_adapter.cc
 * \brief BluetoothLeransportAdapter class source file.
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

#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <iomanip>
#include <set>

#include "transport_manager/bluetooth_le/bluetooth_le_transport_adapter.h"
#include "transport_manager/bluetooth_le/bluetooth_le_device_scanner.h"
#include "transport_manager/bluetooth_le/bluetooth_le_connection_factory.h"

#include "transport_manager/bluetooth_le/ble_client.h"
#include "transport_manager/bluetooth_le/ble_server.h"

#include "utils/logger.h"

namespace transport_manager {
namespace transport_adapter {

SDL_CREATE_LOG_VARIABLE("TransportManager")

BleClient ble_client;
BleServer ble_server;

BluetoothLeTransportAdapter::BluetoothLeTransportAdapter(
    resumption::LastStateWrapperPtr last_state_wrapper,
    const TransportManagerSettings& settings)
    : TransportAdapterImpl(
          new BluetoothLeDeviceScanner(this, true, 0, settings.bluetooth_uuid()),
          new BluetoothLeConnectionFactory(this),
          NULL,
          last_state_wrapper,
          settings) 
          {
            std::thread ble_server_thread([&] {ble_server.Init();
                ble_server.Run();});
            std::thread ble_client_thread([&] {ble_client.Init();
                ble_client.Run();});
            ble_server_thread.join();
            ble_client_thread.join();
          }

DeviceType BluetoothLeTransportAdapter::GetDeviceType() const {
  return BLUETOOTH_LE;
}

void BluetoothLeTransportAdapter::Store() const {
  SDL_LOG_TRACE("enter");
  //TODO: implement
}

bool BluetoothLeTransportAdapter::Restore() {
  SDL_LOG_TRACE("enter");
  //TODO: implement
  return true;
}

}  // namespace transport_adapter
}  // namespace transport_manager
