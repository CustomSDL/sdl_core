package org.luxoft.sdl_core;

import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.le.ScanResult;
import android.content.Context;
import android.os.Handler;
import android.util.Log;

import java.util.ArrayList;
import java.util.UUID;

import static android.bluetooth.BluetoothGattCharacteristic.PROPERTY_WRITE;

class BluetoothHandler {
    public BluetoothCentralManager central;
    private static BluetoothHandler instance = null;
    private BluetoothPeripheral mPeripheral = null;
    private final Context context;
    private final Handler handler = new Handler();

    // Testing-only service with ability to notify and write
    private static final UUID SDL_TESTER_SERVICE_UUID = UUID
            .fromString("00001101-0000-1000-8000-00805f9b34fb");

    // Testing-only characteristic with permissions to write
    private static final UUID MOBILE_RESPONSE_CHARACTERISTIC = UUID
            .fromString("00001103-0000-1000-8000-00805f9b34fb");

    // Testing-only characteristic for notifications
    private static final UUID MOBILE_REQUEST_CHARACTERISTIC = UUID
            .fromString("00001102-0000-1000-8000-00805f9b34fb");


    public static synchronized BluetoothHandler getInstance(Context context) {
        if (instance == null) {
            instance = new BluetoothHandler(context);
        }
        return instance;
    }

    private final BluetoothPeripheralCallback peripheralCallback = new BluetoothPeripheralCallback() {
        @Override
        public void onServicesDiscovered(BluetoothPeripheral peripheral) {
            // Try to turn on notification
            peripheral.setNotify(SDL_TESTER_SERVICE_UUID, MOBILE_REQUEST_CHARACTERISTIC, true);
        }

        @Override
        public void onCharacteristicWrite(BluetoothPeripheral peripheral, byte[] value, BluetoothGattCharacteristic characteristic, GattStatus status) {
            if (status == GattStatus.SUCCESS) {
                Log.i(BleCentralService.TAG, "SUCCESS: Writing " + String.valueOf(value) + " to " + characteristic.getUuid());
            } else {
                Log.i(BleCentralService.TAG, "ERROR: Failed writing " + String.valueOf(value) + " to " + characteristic.getUuid() + " with " + status);
            }
        }

        @Override
        public void onCharacteristicUpdate(BluetoothPeripheral peripheral, byte[] value, BluetoothGattCharacteristic characteristic, GattStatus status) {
            if (status != GattStatus.SUCCESS) return;

            UUID characteristicUUID = characteristic.getUuid();
            if (characteristicUUID.equals(MOBILE_REQUEST_CHARACTERISTIC)) {
                    String msg = characteristic.getStringValue(0);
                    Log.d(BleCentralService.TAG, "message: " + msg);
                    Handler handler = new Handler();
                    handler.postDelayed(new Runnable() {
                        public void run() {
                            writeMessage(peripheral, "Response to: " + msg);
                        }
                }, 3000);
            }
        }
    };

    // Callback for central
    private final BluetoothCentralManagerCallback bluetoothCentralManagerCallback = new BluetoothCentralManagerCallback() {

        @Override
        public void onConnectedPeripheral(BluetoothPeripheral peripheral) {
            Log.i(BleCentralService.TAG, "connected to " + peripheral.getName());
            mPeripheral = peripheral;
        }

        @Override
        public void onConnectionFailed(BluetoothPeripheral peripheral, final HciStatus status) {
            Log.e(BleCentralService.TAG, "connection " + peripheral.getName() + " failed with status " + status);
        }

        @Override
        public void onDisconnectedPeripheral(final BluetoothPeripheral peripheral, final HciStatus status) {
            Log.d(BleCentralService.TAG, "Disconnected from " + peripheral.getName());

            if (mPeripheral != null && peripheral.getAddress().equals(mPeripheral.getAddress())) {
                // Reconnect to this device when it becomes available again
                handler.postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        central.autoConnectPeripheral(peripheral, peripheralCallback);
                    }
                }, 5000);
            }
        }

        @Override
        public void onDiscoveredPeripheral(BluetoothPeripheral peripheral, ScanResult scanResult) {
            Log.v(BleCentralService.TAG, "Found peripheral " + peripheral.getName());
            central.stopScan();
            central.connectPeripheral(peripheral, peripheralCallback);
        }
    };

    private void writeMessage(BluetoothPeripheral peripheral, String message){
        // Hardcoded UUIDs of characteristics, which are suitable for testing
        BluetoothGattCharacteristic responseCharacteristic = peripheral.getCharacteristic(SDL_TESTER_SERVICE_UUID, MOBILE_RESPONSE_CHARACTERISTIC);
        if (responseCharacteristic != null) {
            if ((responseCharacteristic.getProperties() & PROPERTY_WRITE) > 0) {
                Log.d(BleCentralService.TAG, "response: " + message);
                byte[] byte_response = message.getBytes();
                peripheral.writeCharacteristic(responseCharacteristic, byte_response, WriteType.WITH_RESPONSE);
            }
        }
    }

    private BluetoothHandler(Context context) {
        // Create BluetoothCentral
        this.context = context;
    }

    public void disconnect() {
        Log.d(BleCentralService.TAG, "Closing bluetooth handler...");
        handler.removeCallbacksAndMessages(null);
        if (central != null) {
            if (mPeripheral != null) {
                central.cancelConnection(mPeripheral);
                mPeripheral = null;
            }
            central.close();
            central = null;
        }
    }

    public void connect() {
        Log.d(BleCentralService.TAG, "Prepare to start scanning...");
        central = new BluetoothCentralManager(context, bluetoothCentralManagerCallback, new Handler());

        // Scan for peripherals with a certain service UUIDs
        //central.startPairingPopupHack();
        handler.postDelayed(new Runnable() {
            @Override
            public void run() {
                Log.d(BleCentralService.TAG, "Searching for SDL-compatible peripherals...");
                UUID[] servicesToSearch = new UUID[1];
                servicesToSearch[0] = SDL_TESTER_SERVICE_UUID;
                central.scanForPeripheralsWithServices(servicesToSearch);
            }
        }, 1000);
    }
}
