package org.luxoft.sdl_core;

import android.net.LocalServerSocket;
import android.net.LocalSocket;
import android.net.LocalSocketAddress;
import android.util.Log;

import java.io.IOException;
import java.io.InputStream;

public class BleLocalSocketServer extends Thread {
        public static final String TAG = BleCentralService.class.getSimpleName();
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

        public BleLocalSocketServer() {
            Log.d(TAG, " +++ Begin of localSocketServer() +++ ");
            buffer = new byte[bufferSize];
            bytesRead = 0;
            totalBytesRead = 0;
            posOffset = 0;

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

        public void run() {
            Log.d(TAG, " +++ Begin of run() +++ ");
            while (!stopThread) {

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

    }

