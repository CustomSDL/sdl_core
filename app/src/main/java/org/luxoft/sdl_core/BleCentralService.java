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
        public final static String GET_CLIENT_MESSAGE = "GET_CLIENT_MESSAGE";
        public final static String ON_BLE_READY = "ON_BLE_READY";
        public final static String ON_MOBILE_REQUEST_RECEIVED = "ON_MOBILE_REQUEST_RECEIVED";
        //public static String SOCKET_ADDRESS = "./localServer";
        BleLocalSocketServer mLocalSocketServer;
        BleLocalSocketClient mLocalSocketClient;
        BluetoothHandler mBluetoothHandler;

        @Override
        public void onCreate() {
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

        private void initBleLocalSocketServer(){
            mLocalSocketServer = BleLocalSocketServer.getInstance(this);
            mLocalSocketServer.start();
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
                        break;

                    case ACTION_STOP_BLE:
                        Log.i(TAG, "ACTION_STOP_BLE received by centralServiceReceiver");
                        mBluetoothHandler.disconnect();
                        mLocalSocketServer.setStopThread(true);
                        mLocalSocketServer.setStopThread(true);
                        break;

                    case GET_CLIENT_MESSAGE:
                        Log.i(TAG, "GET_CLIENT_MESSAGE received by centralServiceReceiver");
                        String message = mLocalSocketServer.GetMessageFromClient();
                        mBluetoothHandler.writeMessage(message);
                        break;

                    case ON_BLE_READY:
                        Log.i(TAG, "ON_BLE_READY received by centralServiceReceiver");
                        initBleLocalSocketServer();
                        mLocalSocketClient = new BleLocalSocketClient();
                        mLocalSocketClient.start();
                        break;

                    case ON_MOBILE_REQUEST_RECEIVED:
                        Log.i(TAG, "ON_MOBILE_REQUEST_RECEIVED received by centralServiceReceiver");
                        String mobile_request = mBluetoothHandler.GetMobileRequest();
                        mLocalSocketServer.SendMobileRequest(mobile_request);
                        break;
                }
            }
        };

    private static IntentFilter makeCentralServiceIntentFilter() {
        final IntentFilter intentFilter = new IntentFilter();
        intentFilter.addAction(ACTION_START_BLE);
        intentFilter.addAction(ACTION_STOP_BLE);
        intentFilter.addAction(GET_CLIENT_MESSAGE);
        intentFilter.addAction(ON_BLE_READY);
        intentFilter.addAction(ON_MOBILE_REQUEST_RECEIVED);
        return intentFilter;
    }
}

