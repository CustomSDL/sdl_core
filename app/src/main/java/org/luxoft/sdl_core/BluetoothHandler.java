package org.luxoft.sdl_core;

import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.le.ScanResult;
import android.content.Context;
import android.os.Handler;
import android.util.Log;

import java.util.UUID;

import static org.luxoft.sdl_core.BluetoothBytesParser.bytes2String;

class BluetoothHandler {
    public BluetoothCentralManager central;
    private static BluetoothHandler instance = null;
    private final Context context;
    private final Handler handler = new Handler();

    // Testing-only service with ability to notify and write
    private static final UUID HEART_RATE_SERVICE_UUID = UUID
            .fromString("0000180D-0000-1000-8000-00805f9b34fb");

    // Testing-only characteristic with permissions to write
    private static final UUID HEART_RATE_CONTROL_POINT_UUID = UUID
            .fromString("00002A39-0000-1000-8000-00805f9b34fb");

    // Testing-only characteristic for notifications
    private static final UUID HEART_RATE_MEASUREMENT_UUID = UUID
            .fromString("00002A37-0000-1000-8000-00805f9b34fb");


    public static synchronized BluetoothHandler getInstance(Context context) {
        if (instance == null) {
            instance = new BluetoothHandler(context);
        }
        return instance;
    }

    private final BluetoothPeripheralCallback peripheralCallback = new BluetoothPeripheralCallback() {
        @Override
        public void onServicesDiscovered(BluetoothPeripheral peripheral) {
            // Request a higher MTU, iOS always asks for 185
            //peripheral.requestMtu(185);

            // Request a new connection priority
            //peripheral.requestConnectionPriority(ConnectionPriority.HIGH);

            // Try to turn on notifications for other characteristics
            peripheral.setNotify(HEART_RATE_SERVICE_UUID, HEART_RATE_MEASUREMENT_UUID, true);
        }
/*
        @Override
        public void onNotificationStateUpdate(BluetoothPeripheral peripheral, BluetoothGattCharacteristic characteristic, GattStatus status) {
            if (status == GattStatus.SUCCESS) {
                final boolean isNotifying = peripheral.isNotifying(characteristic);
                Timber.i("SUCCESS: Notify set to '%s' for %s", isNotifying, characteristic.getUuid());
                if (characteristic.getUuid().equals(CONTOUR_CLOCK)) {
                    writeContourClock(peripheral);
                } else if (characteristic.getUuid().equals(GLUCOSE_RECORD_ACCESS_POINT_CHARACTERISTIC_UUID)) {
                    writeGetAllGlucoseMeasurements(peripheral);
                }
            } else {
                Timber.e("ERROR: Changing notification state failed for %s (%s)", characteristic.getUuid(), status);
            }
        }*/

        @Override
        public void onCharacteristicWrite(BluetoothPeripheral peripheral, byte[] value, BluetoothGattCharacteristic characteristic, GattStatus status) {
            if (status == GattStatus.SUCCESS) {
                Log.i(BleCentralService.TAG, "SUCCESS: Writing " + bytes2String(value) + " to " + characteristic.getUuid());
            } else {
                Log.i(BleCentralService.TAG, "ERROR: Failed writing " + bytes2String(value) + " to " + characteristic.getUuid() + " with " + status);
            }
        }

        @Override
        public void onCharacteristicUpdate(BluetoothPeripheral peripheral, byte[] value, BluetoothGattCharacteristic characteristic, GattStatus status) {
            if (status != GattStatus.SUCCESS) return;

            UUID characteristicUUID = characteristic.getUuid();
            BluetoothBytesParser parser = new BluetoothBytesParser(value);

            if (characteristicUUID.equals(HEART_RATE_MEASUREMENT_UUID)) {
                /*BloodPressureMeasurement measurement = new BloodPressureMeasurement(value);
                Intent intent = new Intent(MEASUREMENT_BLOODPRESSURE);
                intent.putExtra(MEASUREMENT_BLOODPRESSURE_EXTRA, measurement);
                sendMeasurement(intent, peripheral);*/

                    int flag = characteristic.getProperties();
                    int format = -1;

                    if ((flag & 0x01) != 0) {
                        format = BluetoothGattCharacteristic.FORMAT_UINT16;
                        Log.d(BleCentralService.TAG, "data format UINT16.");
                    } else {
                        format = BluetoothGattCharacteristic.FORMAT_UINT8;
                        Log.d(BleCentralService.TAG, "data format UINT16.");
                    }

                    int msg = characteristic.getIntValue(format, 0);
                    Log.d(BleCentralService.TAG, String.format("message: %d", msg));
                    //intent.putExtra(EXTRA_DATA, msg);
            }
        }

        /*@Override
        public void onMtuChanged(@NotNull BluetoothPeripheral peripheral, int mtu, @NotNull GattStatus status) {
            Timber.i("new MTU set: %d", mtu);
        }*/
    };

    // Callback for central
    private final BluetoothCentralManagerCallback bluetoothCentralManagerCallback = new BluetoothCentralManagerCallback() {

        @Override
        public void onConnectedPeripheral(BluetoothPeripheral peripheral) {
            Log.i(BleCentralService.TAG, "connected to " + peripheral.getName());
        }

        @Override
        public void onConnectionFailed(BluetoothPeripheral peripheral, final HciStatus status) {
            //Timber.e("connection '%s' failed with status %s", peripheral.getName(), status);
        }

        @Override
        public void onDisconnectedPeripheral(final BluetoothPeripheral peripheral, final HciStatus status) {
            //Timber.i("disconnected '%s' with status %s", peripheral.getName(), status);

            // Reconnect to this device when it becomes available again
            handler.postDelayed(new Runnable() {
                @Override
                public void run() {
                    central.autoConnectPeripheral(peripheral, peripheralCallback);
                }
            }, 5000);

        }

        @Override
        public void onDiscoveredPeripheral(BluetoothPeripheral peripheral, ScanResult scanResult) {
            Log.v(BleCentralService.TAG, "Found peripheral " + peripheral.getName());
            central.stopScan();
            central.connectPeripheral(peripheral, peripheralCallback);
        }
    };

    private BluetoothHandler(Context context) {
        // Create BluetoothCentral
        this.context = context;
        central = new BluetoothCentralManager(context, bluetoothCentralManagerCallback, new Handler());

        // Scan for peripherals with a certain service UUIDs
        //central.startPairingPopupHack();
        handler.postDelayed(new Runnable() {
            @Override
            public void run() {
                central.scanForPeripherals();
            }
        }, 1000);
    }
}
