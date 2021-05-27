package org.luxoft.sdl_core;

import android.net.LocalServerSocket;
import android.net.LocalSocket;
import android.util.Log;

import java.io.IOException;
import java.io.InputStream;

public class BleLocalSocketReader implements BleReader {
    public static final String TAG = BleLocalSocketReader.class.getSimpleName();
    LocalServerSocket mServer;
    LocalSocket mReceiver;
    InputStream mInputStream;
    int mBytesRead;
    public static String SOCKET_ADDRESS = "./localBleReader";

    @Override
    public void Connect(){
        Log.i(TAG, "Connect BleLocalSocketReader");
        try {
            mServer = new LocalServerSocket(SOCKET_ADDRESS);
        } catch (IOException e) {
            Log.e(TAG, "The localSocketServer creation failed");
            e.printStackTrace();
        }

        try {
            Log.d(TAG, "BleLocalSocketReader begins to accept()");
            mReceiver = mServer.accept();
        } catch (IOException e) {
            Log.e(TAG, "BleLocalSocketReader accept() failed");
            e.printStackTrace();
        }
    };

    @Override
    public void Disconnect(){
        Log.i(TAG, "Disconnect BleLocalSocketReader");
        if (mReceiver != null){
            try {
                mReceiver.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
        if (mServer != null){
            try {
                mServer.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    };

    @Override
    public void Read(BleAdapterMessageCallback callback){
        Log.i(TAG, "Going to read message");
        try {
            mInputStream = mReceiver.getInputStream();
        } catch (IOException e) {
            Log.e(TAG, "getInputStream() failed");
            e.printStackTrace();
        }

        Log.d(TAG, "The client connect to BleLocalSocketReader");

        while (true) {
            byte[] buffer;
            int bufferSize = 32;
            buffer = new byte[bufferSize];
            try {
                mBytesRead = mInputStream.read(buffer);
            } catch (IOException e) {
                Log.d(TAG, "There is an exception when reading socket");
                e.printStackTrace();
                break;
            }

            if (mBytesRead >= 0) {
                Log.d(TAG, "Receive data from socket, bytesRead = "
                        + mBytesRead);
                String readed_bytes = new String(buffer);
                Log.d(TAG, "Receive data from socket = "
                        + readed_bytes);
                callback.OnMessageReceived(buffer);
            }
        }
    }
}
