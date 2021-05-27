package org.luxoft.sdl_core;

import android.net.LocalSocket;
import android.net.LocalSocketAddress;
import android.util.Log;

import java.io.IOException;

public class BleLocalSocketWriter implements BleWriter{
    public static final String TAG = BleLocalSocketWriter.class.getSimpleName();
    LocalSocket mSocket;
    public static String SOCKET_ADDRESS = "./localBleWriter";

    @Override
    public void Connect(){
        Log.i(TAG, "Connect BleLocalSocketWriter");
        try{
            mSocket = new LocalSocket();
            mSocket.connect(new LocalSocketAddress(SOCKET_ADDRESS));
        } catch (IOException e) {
            Log.e(TAG, "Cannot connect to socket");
        }
    };

    @Override
    public void Disconnect(){
        Log.i(TAG, "Disconnect BleLocalSocketWriter");
        try {
            mSocket.close();
        } catch (IOException e) {
            Log.e(TAG, "Cannot close socket");
            e.printStackTrace();
        }
    };

    @Override
    public void Write(byte[] rawMessage){
        Log.i(TAG, "Going to write message");
        try {
            String str = new String(rawMessage);
            Log.d(TAG, "Write raw message: " + str);
            mSocket.getOutputStream().write(rawMessage);
            mSocket.getOutputStream().close();
        } catch (IOException e) {
            Log.e(TAG, "Cannot write to output stream");
            e.printStackTrace();
        }
    };
}
