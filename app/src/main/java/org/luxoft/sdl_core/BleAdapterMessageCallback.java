package org.luxoft.sdl_core;

public abstract class BleAdapterMessageCallback {
    public void OnMessageReceived(byte[] rawMessage){
        throw new UnsupportedOperationException("Method OnMessageReceived not overridden");
    }
}
