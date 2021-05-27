package org.luxoft.sdl_core;

public abstract class BleReader {
    public void Connect(){
        throw new UnsupportedOperationException("Method Connect not overridden");
    };

    public void Disconnect(){
        throw new UnsupportedOperationException("Method Disconnect not overridden");
    };

    public void Read(BleAdapterMessageCallback callback){
        throw new UnsupportedOperationException("Method Read not overridden");
    };
}
