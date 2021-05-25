package org.luxoft.sdl_core;

import android.content.Context;
import android.content.Intent;
import android.net.LocalServerSocket;
import android.net.LocalSocket;
import android.net.LocalSocketAddress;
import android.util.Log;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import static org.luxoft.sdl_core.BleCentralService.ACTION_START_BLE;
import static org.luxoft.sdl_core.BleCentralService.GET_CLIENT_MESSAGE;

public class BleLocalSocketServer extends Thread {
        public static final String TAG = BleLocalSocketServer.class.getSimpleName();
        int bufferSize = 32;
        byte[] buffer;
        int bytesRead;
        int totalBytesRead;
        int posOffset;
        LocalServerSocket server;
        LocalSocket receiver;
        InputStream input;
        private volatile boolean stopThread;
        public static String SOCKET_ADDRESS = "./localServer";
        String message;
        private final Context context;
        private static BleLocalSocketServer instance = null;

        public String GetMessageFromClient(){
            return message;
        }

        public void run() {
            while (!stopThread) {
                Log.d(TAG, " +++ One server iteration +++ ");
                if (null == server){
                    Log.d(TAG, "The localSocketServer is NULL !!!");
                    stopThread = true;
                    break;
                }

                try {
                    Log.d(TAG, "localSocketServer begins to accept()");
                    receiver = server.accept();
                } catch (IOException e) {
                    // TODO Auto-generated catch block
                    Log.d(TAG, "localSocketServer accept() failed !!!");
                    e.printStackTrace();
                    continue;
                }

                try {
                    input = receiver.getInputStream();
                } catch (IOException e) {
                    // TODO Auto-generated catch block
                    Log.d(TAG, "getInputStream() failed !!!");
                    e.printStackTrace();
                    continue;
                }

                Log.d(TAG, "The client connect to LocalServerSocket");

                while (receiver != null) {
                    try {
                        bytesRead = input.read(buffer, posOffset,
                                (bufferSize - totalBytesRead));
                    } catch (IOException e) {
                        // TODO Auto-generated catch block
                        Log.d(TAG, "There is an exception when reading socket");
                        e.printStackTrace();
                        break;
                    }

                    if (bytesRead >= 0) {
                        Log.d(TAG, "Receive data from socket, bytesRead = "
                                + bytesRead);
                        posOffset += bytesRead;
                        totalBytesRead += bytesRead;
                        String readed_bytes = new String(buffer);
                        Log.d(TAG, "Receive data from socket = "
                                + readed_bytes);
                        message = readed_bytes;
                        final Intent intent = new Intent(GET_CLIENT_MESSAGE);
                        context.sendBroadcast(intent);

                    }

                    if (totalBytesRead == bufferSize) {
                        Log.d(TAG, "The buffer is full");
                        String str = new String(buffer);
                        Log.d(TAG, "The context of buffer is : " + str);

                        bytesRead = 0;
                        totalBytesRead = 0;
                        posOffset = 0;
                    }

                }
                Log.d(TAG, "The client socket is null");
            }
            Log.d(TAG, "The LocalSocketServer thread is going to stop");
            if (receiver != null){
                try {
                    receiver.close();
                } catch (IOException e) {
                    // TODO Auto-generated catch block
                    e.printStackTrace();
                }
            }
            if (server != null){
                try {
                    server.close();
                } catch (IOException e) {
                    // TODO Auto-generated catch block
                    e.printStackTrace();
                }
            }
        }

        public void setStopThread(boolean value){
            stopThread = value;
            Thread.currentThread().interrupt(); // TODO : Check
        }

        public void SendMobileRequest(String mobile_request){
            Log.d(TAG, "mobile_request inside server: " + mobile_request);
        }

        private BleLocalSocketServer(Context context) {
            Log.d(TAG, " +++ Begin of localSocketServer() +++ ");
            buffer = new byte[bufferSize];
            bytesRead = 0;
            totalBytesRead = 0;
            posOffset = 0;
            this.context = context;

            try {
                server = new LocalServerSocket(SOCKET_ADDRESS);
            } catch (IOException e) {
                // TODO Auto-generated catch block
                Log.d(TAG, "The localSocketServer created failed !!!");
                e.printStackTrace();
            }

            LocalSocketAddress localSocketAddress;
            localSocketAddress = server.getLocalSocketAddress();
            String str = localSocketAddress.getName();

            Log.d(TAG, "The LocalSocketAddress = " + str);

            stopThread = false;
        }

    public static synchronized BleLocalSocketServer getInstance(Context context) {
        if (instance == null) {
            instance = new BleLocalSocketServer(context);
        }
        return instance;
    }

    }

