package org.luxoft.sdl_core;

import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.le.ScanResult;
import android.content.Context;
import android.content.Intent;
import android.os.Handler;
import android.util.Log;

import com.welie.blessed.BluetoothCentralManager;
import com.welie.blessed.BluetoothCentralManagerCallback;
import com.welie.blessed.BluetoothPeripheral;
import com.welie.blessed.BluetoothPeripheralCallback;
import com.welie.blessed.GattStatus;
import com.welie.blessed.HciStatus;
import com.welie.blessed.WriteType;

import java.util.UUID;

import static android.bluetooth.BluetoothGattCharacteristic.PROPERTY_WRITE;
import static org.luxoft.sdl_core.BleCentralService.MOBILE_DATA_EXTRA;
import static org.luxoft.sdl_core.BleCentralService.ON_BLE_READY;
import static org.luxoft.sdl_core.BleCentralService.ON_MOBILE_MESSAGE_RECEIVED;

class BluetoothHandler {
    public BluetoothCentralManager central;
    private static BluetoothHandler instance = null;
    private BluetoothPeripheral mPeripheral = null;
    private final Context context;
    private final Handler handler = new Handler();

    public static final String TAG = BluetoothHandler.class.getSimpleName();

    // To request a higher MTU, iOS always asks for 185
    public static final int PREFERRED_MTU = 185;

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

            peripheral.requestMtu(PREFERRED_MTU);

            // Try to turn on notification
            peripheral.setNotify(SDL_TESTER_SERVICE_UUID, MOBILE_REQUEST_CHARACTERISTIC, true);

            final Intent intent = new Intent(ON_BLE_READY);
            context.sendBroadcast(intent);
        }

        @Override
        public void onCharacteristicWrite(BluetoothPeripheral peripheral, byte[] value, BluetoothGattCharacteristic characteristic, GattStatus status) {
            if (status == GattStatus.SUCCESS) {
                Log.i(TAG, "SUCCESS: Writing " + String.valueOf(value) + " to " + characteristic.getUuid());
            } else {
                Log.i(TAG, "ERROR: Failed writing " + String.valueOf(value) + " to " + characteristic.getUuid() + " with " + status);
            }
        }

        @Override
        public void onCharacteristicUpdate(BluetoothPeripheral peripheral, byte[] value, BluetoothGattCharacteristic characteristic, GattStatus status) {
            if (status != GattStatus.SUCCESS) return;

            UUID characteristicUUID = characteristic.getUuid();
            if (characteristicUUID.equals(MOBILE_REQUEST_CHARACTERISTIC)) {
                    String msg = characteristic.getStringValue(0);
                    Log.d(TAG, "message: " + msg);
                    byte[] msg_value = characteristic.getValue();
                    final Intent intent = new Intent(ON_MOBILE_MESSAGE_RECEIVED);
                    intent.putExtra(MOBILE_DATA_EXTRA, msg_value);
                    context.sendBroadcast(intent);

            }
        }
    };

    // Callback for central
    private final BluetoothCentralManagerCallback bluetoothCentralManagerCallback = new BluetoothCentralManagerCallback() {

        @Override
        public void onConnectedPeripheral(BluetoothPeripheral peripheral) {
            Log.i(TAG, "connected to " + peripheral.getName());
            mPeripheral = peripheral;
        }

        @Override
        public void onConnectionFailed(BluetoothPeripheral peripheral, final HciStatus status) {
            Log.e(TAG, "connection " + peripheral.getName() + " failed with status " + status);
        }

        @Override
        public void onDisconnectedPeripheral(final BluetoothPeripheral peripheral, final HciStatus status) {
            Log.d(TAG, "Disconnected from " + peripheral.getName());

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
            Log.v(TAG, "Found peripheral " + peripheral.getName());
            central.stopScan();
            central.connectPeripheral(peripheral, peripheralCallback);
        }
    };

    public void writeMessage(byte[] message){

        if (mPeripheral == null) {
            Log.e(TAG, "mPeripheral is null");
            return;
        }

        // Hardcoded UUIDs of characteristics, which are suitable for testing
        BluetoothGattCharacteristic responseCharacteristic = mPeripheral.getCharacteristic(SDL_TESTER_SERVICE_UUID, MOBILE_RESPONSE_CHARACTERISTIC);
        if (responseCharacteristic != null) {
            if ((responseCharacteristic.getProperties() & PROPERTY_WRITE) > 0) {
                Log.d(TAG, "response: " + message.toString());
                mPeripheral.writeCharacteristic(responseCharacteristic, message, WriteType.WITH_RESPONSE);
            }
        }
    }

    private BluetoothHandler(Context context) {
        // Create BluetoothCentral
        this.context = context;
    }

    public void disconnect() {
        Log.d(TAG, "Closing bluetooth handler...");
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
        Log.d(TAG, "Prepare to start scanning...");
        central = new BluetoothCentralManager(context, bluetoothCentralManagerCallback, new Handler());

        // Scan for peripherals with a certain service UUIDs
        handler.postDelayed(new Runnable() {
            @Override
            public void run() {
                Log.d(TAG, "Searching for SDL-compatible peripherals...");
                UUID[] servicesToSearch = new UUID[1];
                servicesToSearch[0] = SDL_TESTER_SERVICE_UUID;
                central.scanForPeripheralsWithServices(servicesToSearch);
            }
        }, 1000);
    }
}
