package org.luxoft.sdl_core;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;

public class BleCentralService extends Service {
        public static final String TAG = BleCentralService.class.getSimpleName();

        @Override
        public void onCreate() {
            super.onCreate();
        }

        @Override
        public void onDestroy() {
            //TODO: perform disconnect before calling onDestroy
            BluetoothHandler handler = BluetoothHandler.getInstance(this);
            handler.disconnect();
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
            initBluetoothHandler();
            return super.onStartCommand(intent, flags, startId);
        }

}

