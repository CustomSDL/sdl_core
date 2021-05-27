package org.luxoft.sdl_core;

import android.os.Handler;
import android.os.Looper;
import android.os.Message;

public class JavaToNativeBleAdapter extends Thread {
    public static final String TAG = JavaToNativeBleAdapter.class.getSimpleName();
    final int WRITE_ID = 1;
    final int READ_ID = 2;

    Handler mHandler;
    BleWriter mWriter;
    BleReader mReader;

    JavaToNativeBleAdapter(){
        mWriter = new BleLocalSocketWriter();
        mReader = new BleLocalSocketReader();
    }

    public void ForwardMessageToNative(byte[] rawMessage){
        Message message = mHandler.obtainMessage(WRITE_ID, rawMessage);
        mHandler.sendMessage(message);
    }

    public void ReadMessageFromNative(BleAdapterMessageCallback callback){
        Message message = mHandler.obtainMessage(READ_ID, callback);
        mHandler.sendMessage(message);
    }

    public void run() {
        mWriter.Connect();
        mReader.Connect();
        Looper.prepare();

        mHandler = new Handler(Looper.myLooper()) {
            public void handleMessage(Message msg) {
                switch (msg.what) {
                    case WRITE_ID:
                        mWriter.Write((byte[]) msg.obj);
                        break;
                    case READ_ID:
                        mReader.Read((BleAdapterMessageCallback) msg.obj);
                        break;
                }
            }
        };
        Looper.loop();
        mWriter.Disconnect();
        mReader.Disconnect();
    }

    public void setStopThread(boolean value){
        //stopThread = value;
        Thread.currentThread().interrupt();
    }

}
