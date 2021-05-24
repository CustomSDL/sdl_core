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
        //public static String SOCKET_ADDRESS = "./localServer";
        BleLocalSocketServer mLocalSocketServer;
        BleLocalSocketClient mLocalSocketClient;

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
            BluetoothHandler handler = BluetoothHandler.getInstance(this);
            handler.connect();
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
                        mLocalSocketServer = new BleLocalSocketServer();
                        mLocalSocketServer.start();
                        mLocalSocketClient = new BleLocalSocketClient();
                        mLocalSocketClient.start();
                        break;

                    case ACTION_STOP_BLE:
                        Log.i(TAG, "ACTION_STOP_BLE received by centralServiceReceiver");
                        BluetoothHandler handler = BluetoothHandler.getInstance(context);
                        handler.disconnect();
                        mLocalSocketServer.setStopThread(true);
                        mLocalSocketServer.setStopThread(true);
                        break;
                }
            }
        };

    private static IntentFilter makeCentralServiceIntentFilter() {
        final IntentFilter intentFilter = new IntentFilter();
        intentFilter.addAction(ACTION_START_BLE);
        intentFilter.addAction(ACTION_STOP_BLE);
        return intentFilter;
    }
}

