package org.luxoft.sdl_core;

public interface BleWriter {
    void Connect();
    void Disconnect();
    void Write(byte[] rawMessage);
}
