package org.luxoft.sdl_core;

public interface BleAdapterMessageCallback {
    void OnMessageReceived(byte[] rawMessage);
}
