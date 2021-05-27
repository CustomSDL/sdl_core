package org.luxoft.sdl_core;

public abstract class BleWriter {
    public void Connect(){
        throw new UnsupportedOperationException("Method Connect not overridden");
    };

    public void Disconnect(){
        throw new UnsupportedOperationException("Method Disconnect not overridden");
    };

    public void Write(byte[] rawMessage){
        throw new UnsupportedOperationException("Method Write not overridden");
    };
}
