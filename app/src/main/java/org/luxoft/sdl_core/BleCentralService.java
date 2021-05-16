package org.luxoft.sdl_core;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;

public class BleCentralService extends Service {
        public static final String TAG = "BleCentralService";

        @Override
        public void onCreate() {
            super.onCreate();
        }

        @Override
        public IBinder onBind(Intent intent) {
            return null;
        }

        private void initBluetoothHandler(){
            BluetoothHandler.getInstance(this);
        }

        @Override
        public int onStartCommand(Intent intent, int flags, int startId) {
            initBluetoothHandler();
            return super.onStartCommand(intent, flags, startId);
        }

}

