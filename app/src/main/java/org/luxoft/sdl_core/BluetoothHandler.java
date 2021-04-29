package org.luxoft.sdl_core;

//import android.content.Context;
import android.os.Handler;
import android.util.Log;

import java.util.UUID;

class BluetoothHandler {
    public BluetoothCentralManager central;
    private static BluetoothHandler instance = null;
    private final Handler handler = new Handler();

    public static synchronized BluetoothHandler getInstance() {
        if (instance == null) {
            instance = new BluetoothHandler();
        }
        return instance;
    }
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

    private BluetoothHandler() {
        // Create BluetoothCentral
        central = new BluetoothCentralManager(bluetoothCentralManagerCallback, new Handler());

        // Scan for peripherals with a certain service UUIDs
        //central.startPairingPopupHack();
        handler.postDelayed(new Runnable() {
            @Override
            public void run() {
                central.scanForPeripheralsWithServices(new UUID[]{BLP_SERVICE_UUID, HTS_SERVICE_UUID, HRS_SERVICE_UUID, PLX_SERVICE_UUID, WSS_SERVICE_UUID, GLUCOSE_SERVICE_UUID});
            }
        }, 1000);
    }
}
}
