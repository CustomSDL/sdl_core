package org.luxoft.sdl_core;

import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.util.Log;

public class JavaToNativeBleAdapter extends Thread {
    public static final String TAG = JavaToNativeBleAdapter.class.getSimpleName();
    private static final int WRITE_ID = 1;
    private static final int READ_ID = 2;

    private native static void OnBLEMessageReseived(String ble_msg);

    Handler mHandler;
    BleWriter mWriter;
    BleReader mReader;
    BleAdapterMessageCallback mCallback;

    JavaToNativeBleAdapter(){
        mWriter = new BleLocalSocketWriter();
        mReader = new BleLocalSocketReader();
    }

    public void ForwardMessageToNative(byte[] rawMessage){
        String stringified_message = new String(rawMessage);
        Log.i(TAG, "Forward message to native: " + stringified_message);
        Message message = mHandler.obtainMessage(WRITE_ID, rawMessage);
        mHandler.sendMessage(message);
    }

    public void ReadMessageFromNative(BleAdapterMessageCallback callback){
        Log.i(TAG, "Save callback to read message from native");
        mCallback = callback;
        Message message = mHandler.obtainMessage(READ_ID, mCallback);
        mHandler.sendMessage(message);
    }

    @Override
    public void run() {
        mWriter.Connect();
        mReader.Connect();
        Looper.prepare();
        mHandler = new Handler(Looper.myLooper()) {
            public void handleMessage(Message msg) {
                switch (msg.what) {
                    case WRITE_ID:
                        mWriter.Write((byte[]) msg.obj);
                        OnBLEMessageReseived(new String((byte[]) msg.obj));
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

    public void setStopThread(){
        mHandler.getLooper().quit();
    }

}
