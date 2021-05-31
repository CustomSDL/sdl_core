package org.luxoft.sdl_core;

import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.IBinder;
import android.util.Log;

public class BleCentralService extends Service {
        public static final String TAG = BleCentralService.class.getSimpleName();
        public final static String ACTION_START_BLE = "ACTION_START_BLE";
        public final static String ACTION_STOP_BLE = "ACTION_STOP_BLE";
        public final static String ON_BLE_READY = "ON_BLE_READY";
        public final static String ON_MOBILE_MESSAGE_RECEIVED = "ON_MOBILE_MESSAGE_RECEIVED";
        public final static String MOBILE_DATA_EXTRA = "MOBILE_DATA_EXTRA";
        BluetoothHandler mBluetoothHandler;
        JavaToNativeBleAdapter mNativeBleAdapter;
        BleAdapterWriteMessageCallback mCallback;

        @Override
        public void onCreate() {
            mNativeBleAdapter = new JavaToNativeBleAdapter();
            super.onCreate();
        }

        @Override
        public void onDestroy() {
            unregisterReceiver(centralServiceReceiver);
            super.onDestroy();
        }

        @Override
        public IBinder onBind(Intent intent) {
            return null;
        }

        private void initBluetoothHandler(){
            mBluetoothHandler = BluetoothHandler.getInstance(this);
            mBluetoothHandler.connect();
        }

        @Override
        public int onStartCommand(Intent intent, int flags, int startId) {
            registerReceiver(centralServiceReceiver, makeCentralServiceIntentFilter());
            return super.onStartCommand(intent, flags, startId);
        }

        private final BroadcastReceiver centralServiceReceiver = new BroadcastReceiver() {
            @Override
            public void onReceive(Context context, Intent intent) {
                String action = intent.getAction();
                if (action == null) {
                    return;
                }

                switch (intent.getAction()) {
                    case ACTION_START_BLE:
                        Log.i(TAG, "ACTION_START_BLE received by centralServiceReceiver");
                        initBluetoothHandler();
                        mNativeBleAdapter.start();
                        break;

                    case ACTION_STOP_BLE:
                        Log.i(TAG, "ACTION_STOP_BLE received by centralServiceReceiver");
                        mBluetoothHandler.disconnect();
                        mNativeBleAdapter.setStopThread();
                        break;

                    case ON_BLE_READY:
                        Log.i(TAG, "ON_BLE_READY received by centralServiceReceiver");
                        mCallback = new BleAdapterWriteMessageCallback();
                        mNativeBleAdapter.ReadMessageFromNative(mCallback);

                        break;

                    case ON_MOBILE_MESSAGE_RECEIVED:
                        Log.i(TAG, "ON_MOBILE_MESSAGE_RECEIVED received by centralServiceReceiver");
                        byte[] mobile_message = intent.getByteArrayExtra(MOBILE_DATA_EXTRA);
                        mNativeBleAdapter.ForwardMessageToNative(mobile_message);
                        break;
                }
            }
        };

    private static IntentFilter makeCentralServiceIntentFilter() {
        final IntentFilter intentFilter = new IntentFilter();
        intentFilter.addAction(ACTION_START_BLE);
        intentFilter.addAction(ACTION_STOP_BLE);
        intentFilter.addAction(ON_BLE_READY);
        intentFilter.addAction(ON_MOBILE_MESSAGE_RECEIVED);
        return intentFilter;
    }

    class BleAdapterWriteMessageCallback implements BleAdapterMessageCallback{
        public void OnMessageReceived(byte[] rawMessage) {
            mBluetoothHandler.writeMessage(rawMessage);
        }
    };
}

