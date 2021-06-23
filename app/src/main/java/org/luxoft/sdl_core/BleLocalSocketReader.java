package org.luxoft.sdl_core;

import android.net.LocalServerSocket;
import android.net.LocalSocket;
import android.util.Log;

import java.io.IOException;
import java.io.InputStream;

import static org.luxoft.sdl_core.BluetoothHandler.PREFERRED_MTU;

public class BleLocalSocketReader implements BleReader {
    public static final String TAG = BleLocalSocketReader.class.getSimpleName();
    LocalServerSocket mServer;
    LocalSocket mReceiver;
    InputStream mInputStream;
    int mBytesRead;
    BleAdapterMessageCallback mCallback;
    Thread mLoopTread;
    public static final int mBufferSize = PREFERRED_MTU;
    public static String SOCKET_ADDRESS = "./localBleReader";


    @Override
    public void Connect(OnConnectCallback callback){
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

        callback.Execute();
    };

    @Override
    public void Disconnect(){
        Log.i(TAG, "Disconnect BleLocalSocketReader");

        mLoopTread.interrupt();
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
        mCallback = callback;
        try {
            mInputStream = mReceiver.getInputStream();
        } catch (IOException e) {
            Log.e(TAG, "getInputStream() failed");
            e.printStackTrace();
        }

        Log.d(TAG, "The client connect to BleLocalSocketReader");

        ReadLoop readLoop = new ReadLoop();
        mLoopTread = new Thread(readLoop);
        mLoopTread.start();
    }

    class ReadLoop implements Runnable {
        @Override
        public void run() {
            while (true) {
                byte[] buffer;
                buffer = new byte[mBufferSize];
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
                    String stringified_data = new String(buffer);
                    Log.d(TAG, "Receive data from socket = "
                            + stringified_data);
                    mCallback.OnMessageReceived(buffer);
                }
            }
        }
    }
}
