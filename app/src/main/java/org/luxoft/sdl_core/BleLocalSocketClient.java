package org.luxoft.sdl_core;

import android.net.LocalSocket;
import android.net.LocalSocketAddress;
import android.util.Log;

import java.io.IOException;
import java.io.InputStream;

public class BleLocalSocketClient extends Thread {

        public static final String TAG = BleLocalSocketClient.class.getSimpleName();
        private volatile boolean stopThread;
        public static String SOCKET_ADDRESS = "./localServer";

        public BleLocalSocketClient(){
            Log.d(TAG, " +++ Begin of localSocketClient() +++ ");
            stopThread = false;
        }

        public void run(){
            Log.d(TAG, " +++ Begin of run() +++ ");
            while(!stopThread){
                // Here may be call to native part with method clientSocketThreadNative();
                // But for now I put some simple test sequence
                //Todo: replace with valid logic
                try {
                    LocalSocket sender = new LocalSocket();
                    sender.connect(new LocalSocketAddress(SOCKET_ADDRESS));
                    String data = "Response from client!";
                    Log.d("SENT DATA", data);
                    sender.getOutputStream().write(data.getBytes());
                    sender.getOutputStream().close();
                    sender.close();
                } catch (IOException ex) {
                    Log.wtf("IOEXCEPTION", ex);
                }
            }
        }

        public void setStopThread(boolean value){
            stopThread = value;
            //setStopThreadNative();
            Thread.currentThread().interrupt(); // TODO : Check
        }
    }
