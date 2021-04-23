package org.luxoft.sdl_core;

import android.app.Service;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCallback;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattService;
import android.bluetooth.BluetoothManager;
import android.bluetooth.BluetoothProfile;
import android.bluetooth.le.BluetoothLeScanner;
import android.bluetooth.le.ScanCallback;
import android.bluetooth.le.ScanFilter;
import android.bluetooth.le.ScanResult;
import android.bluetooth.le.ScanSettings;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.os.IBinder;
import android.util.Log;
import android.os.Handler;
import android.os.Looper;

import java.util.ArrayList;
import java.util.List;

import static android.bluetooth.BluetoothDevice.TRANSPORT_LE;
import static android.bluetooth.BluetoothGatt.GATT_SUCCESS;

public class BleCentralService extends Service {
        /**
         * Stops scanning after 30 seconds.
         */
        private static final long SCAN_PERIOD = 30000;
        public static final String TAG = "BleCentralService";

        private DevicesAdapter mDevicesAdapter;
        private ArrayList<ArrayList<BluetoothGattCharacteristic>> mDeviceServices;
        private ScanCallback mScanCallback;
        private Handler mHandler;
        private BluetoothManager mBluetoothManager;
        private BluetoothAdapter mBluetoothAdapter;
        private BluetoothGatt mBluetoothGatt;
        private ArrayList<BluetoothGattCharacteristic> mCharacteristics;

        public final static String ACTION_GATT_CONNECTED = "ACTION_GATT_CONNECTED";
        public final static String ACTION_GATT_DISCONNECTED = "ACTION_GATT_DISCONNECTED";
        public final static String ACTION_GATT_SERVICES_DISCOVERED = "ACTION_GATT_SERVICES_DISCOVERED";
        public final static String ACTION_DATA_AVAILABLE = "ACTION_DATA_AVAILABLE";
        public final static String EXTRA_DATA = "EXTRA_DATA";

        @Override
        public void onCreate() {
            super.onCreate();
            mDevicesAdapter = new DevicesAdapter();
            mHandler = new Handler(Looper.getMainLooper());
            mDeviceServices = new ArrayList<>();
        }

        @Override
        public IBinder onBind(Intent intent) {
            return null;
        }

        @Override
        public int onStartCommand(Intent intent, int flags, int startId) {
            startBLEScan();
            return super.onStartCommand(intent, flags, startId);
        }

        private void startBLEScan() {
            BluetoothAdapter BleAdapter = BluetoothAdapter.getDefaultAdapter();
            BluetoothLeScanner bluetoothLeScanner = BleAdapter.getBluetoothLeScanner();
            Log.d(TAG, "Starting Scanning");

            mHandler.postDelayed(new Runnable() {
                    @Override public void run() {
                        stopScanning();

                        //You may process here all devices, which were found during scan,
                        // if results were accumulated and scanning wasn't stopped after first result
                        }
                    }, SCAN_PERIOD);

                        // Kick off a new scan.
            mScanCallback = new SampleScanCallback();
            bluetoothLeScanner.startScan(buildScanFilters(), buildScanSettings(), mScanCallback);
        }

        private List<ScanFilter> buildScanFilters() {

            List<ScanFilter> scanFilters = new ArrayList<>();

            ScanFilter.Builder builder = new ScanFilter.Builder();
            // Comment out the below line to see all BLE devices around you
            //builder.setServiceUuid(Constants.SERVICE_UUID);
            scanFilters.add(builder.build());

            return scanFilters;
        }

        private ScanSettings buildScanSettings() {
            ScanSettings.Builder builder = new ScanSettings.Builder();
            builder.setScanMode(ScanSettings.SCAN_MODE_LOW_POWER);
            return builder.build();
        }

        public void stopScanning() {

            Log.d(TAG, "Stopping Scanning");

            BluetoothAdapter BleAdapter = BluetoothAdapter.getDefaultAdapter();

                BluetoothLeScanner bluetoothLeScanner = BleAdapter.getBluetoothLeScanner();

                if (bluetoothLeScanner != null) {
                    bluetoothLeScanner.stopScan(mScanCallback);
                    mScanCallback = null;
                    return;
                }


            }

        private class SampleScanCallback extends ScanCallback {

            @Override
            public void onBatchScanResults(List<ScanResult> results) {
                super.onBatchScanResults(results);
                mDevicesAdapter.add(results);
                logResults(results);
            }

            @Override
            public void onScanResult(int callbackType, ScanResult result) {
                super.onScanResult(callbackType, result);
                mDevicesAdapter.add(result);
                logResults(result);
            }

            @Override
            public void onScanFailed(int errorCode) {
                super.onScanFailed(errorCode);
                Log.v(TAG, "Scan failed with error: " + errorCode);
            }


            private void logResults(List<ScanResult> results) {
                if (results != null) {
                    for (ScanResult result : results) {
                        logResults(result);
                    }
                }
            }

            private void logResults(ScanResult result) {
                if (result != null) {
                    BluetoothDevice device = result.getDevice();
                    if (device != null) {
                        Log.v(TAG, device.getName() + " " + device.getAddress());
                        return;
                    }
                }
                Log.e(TAG, "error SampleScanCallback");
            }
        }

    private final BluetoothGattCallback bluetoothGattCallback = new BluetoothGattCallback() {
        @Override
        public void onConnectionStateChange(BluetoothGatt gatt,
                                            int status,
                                            int newState) {
            String intentAction;
            if (status == GATT_SUCCESS) {
                if (newState == BluetoothProfile.STATE_CONNECTED) {
                    //intentAction = ACTION_GATT_CONNECTED;
                    //mConnectionState = STATE_CONNECTED;
                    Log.i(TAG, "Connected to GATT server.");
                    Log.i(TAG, "Attempting to start service discovery:" + mBluetoothGatt.discoverServices());
                    //broadcastUpdate(intentAction);
                } else if (newState == BluetoothProfile.STATE_DISCONNECTED) {
                    //intentAction = ACTION_GATT_DISCONNECTED;
                    //mConnectionState = STATE_DISCONNECTED;??
                    Log.i(TAG, "Disconnected from GATT server.");
                    //broadcastUpdate(intentAction);
                    gatt.close();
                }
            } else {
                gatt.close();
            }
        }

        @Override
        public void onServicesDiscovered(BluetoothGatt gatt, int status) {
            if (status == BluetoothGatt.GATT_SUCCESS) {
                Log.w(TAG, "onServicesDiscovered: success, broadcasting... ");
                //broadcastUpdate(ACTION_GATT_SERVICES_DISCOVERED);
                List<BluetoothGattService> services = mBluetoothGatt.getServices();
                Log.i(TAG, String.format("Discovered %d services", services.size()));
                setGattServices(services);
                registerCharacteristic();
            } else {
                Log.w(TAG, "onServicesDiscovered received: " + status);
                //disconnect(); ??
            }
        }

        @Override
        public void onCharacteristicRead(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic, int status) {
            if (status == BluetoothGatt.GATT_SUCCESS) {
                broadcastUpdate(ACTION_DATA_AVAILABLE, characteristic);
            } else {
                Log.w(TAG, "onCharacteristicRead GATT_FAILURE");
            }
        }

        @Override
        public void onCharacteristicChanged(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic) {
            broadcastUpdate(ACTION_DATA_AVAILABLE, characteristic);
        }
    };

    private class DevicesAdapter {

        private ArrayList<BluetoothDevice> mScannedDevices = new ArrayList<>();

        public void add(ScanResult scanResult) {
            if (scanResult == null) {
                return;
            }

            int existingPosition = getPosition(scanResult.getDevice().getAddress());

            if (existingPosition >= 0) {
                // Device is already in list, update its record.
                mScannedDevices.set(existingPosition, scanResult.getDevice());
            } else {
                // Add new Device's ScanResult to list.
                mScannedDevices.add(scanResult.getDevice());
            }
            if(!mScannedDevices.isEmpty()){
                BluetoothDevice first_device = mScannedDevices.get(0);
                mBluetoothGatt = first_device.connectGatt(getBaseContext(), false, bluetoothGattCallback, TRANSPORT_LE);
                stopScanning();
            }
        }

        public void add(List<ScanResult> scanResults) {
            if (scanResults != null) {
                for (ScanResult scanResult : scanResults) {
                    add(scanResult);
                }
            }
        }

        private int getPosition(String address) {
            int position = -1;
            for (int i = 0; i < mScannedDevices.size(); i++) {
                if (mScannedDevices.get(i).getAddress().equals(address)) {
                    position = i;
                    break;
                }
            }
            return position;
        }
    }

    private void setGattServices(List<BluetoothGattService> gattServices) {

        if (gattServices == null) {
            return;
        }

        mDeviceServices = new ArrayList<>();

        // Loops through available GATT Services from the connected device
        for (BluetoothGattService gattService : gattServices) {
            ArrayList<BluetoothGattCharacteristic> characteristic = new ArrayList<>();
            characteristic.addAll(gattService.getCharacteristics()); // each GATT Service can have multiple characteristic
            mDeviceServices.add(characteristic);
        }

    }

    private void broadcastUpdate(final String action) {
        final Intent intent = new Intent(action);
        sendBroadcast(intent);
    }

    /*
     Handles various events fired by the Service.
     ACTION_GATT_CONNECTED: connected to a GATT server.
     ACTION_GATT_DISCONNECTED: disconnected from a GATT server.
     ACTION_GATT_SERVICES_DISCOVERED: discovered GATT services.
     ACTION_DATA_AVAILABLE: received data from the device.  This can be a result of read or notification operations.
    */
    private final BroadcastReceiver mGattUpdateReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {

            String action = intent.getAction();

            if (action == null) {
                return;
            }

            switch (intent.getAction()) {
                case ACTION_GATT_SERVICES_DISCOVERED:
                    // set all the supported services and characteristics on the user interface.
                    List<BluetoothGattService> services = mBluetoothGatt.getServices();
                    Log.i(TAG, String.format("Discovered %d services", services.size()));
                    setGattServices(services);
                    registerCharacteristic();
                    break;

                case ACTION_DATA_AVAILABLE:
                    int msg = intent.getIntExtra(EXTRA_DATA, -1);
                    Log.v(TAG, "ACTION_DATA_AVAILABLE " + msg);
                    break;

            }
        }
    };

    private void registerCharacteristic() {

        BluetoothGattCharacteristic characteristic = null;

        if (mDeviceServices != null) {

            /* iterate all the Services the connected device offer.
            a Service is a collection of Characteristic.
             */
            for (ArrayList<BluetoothGattCharacteristic> service : mDeviceServices) {
                for (BluetoothGattCharacteristic serviceCharacteristic : service) {
                    characteristic = serviceCharacteristic;
                }
                }
            }


           /*
            int charaProp = characteristic.getProperties();
            if ((charaProp | BluetoothGattCharacteristic.PROPERTY_READ) > 0) {
            */

            if (characteristic != null) {
                readCharacteristic(characteristic);
                //setCharacteristicNotification(mCharacteristic, true);
            }
        }

    /**
     * Request a read on a given {@code BluetoothGattCharacteristic}. The read result is reported
     * asynchronously through the {@code BluetoothGattCallback#onCharacteristicRead(android.bluetooth.BluetoothGatt, android.bluetooth.BluetoothGattCharacteristic, int)}
     * callback.
     *
     * @param characteristic The characteristic to read from.
     */
    public void readCharacteristic(BluetoothGattCharacteristic characteristic) {

        /*if (mBluetoothAdapter == null || mBluetoothGatt == null) {
            Log.w(TAG, "BluetoothAdapter not initialized");
            return;
        }*/

        mBluetoothGatt.readCharacteristic(characteristic);
    }

    private void broadcastUpdate(final String action, final BluetoothGattCharacteristic characteristic) {

        Intent intent = new Intent(action);
            /*
            for all profiles, writes the data formatted in HEX.
            */
            final byte[] data = characteristic.getValue();

            if (data != null && data.length > 0) {
                final StringBuilder stringBuilder = new StringBuilder(data.length);
                for (byte byteChar : data) {
                    stringBuilder.append(String.format("%02X ", byteChar));
                }

                Log.w(TAG, "broadcastUpdate. general profile");
                String read_data = new String(data) + "\n" + stringBuilder.toString();
                intent.putExtra(EXTRA_DATA, read_data);
                Log.w(TAG, read_data);
            }
        sendBroadcast(intent);
    }


}

