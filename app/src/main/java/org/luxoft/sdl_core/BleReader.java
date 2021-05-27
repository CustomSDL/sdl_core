package org.luxoft.sdl_core;

public interface BleReader {
        void Connect();
        void Disconnect();
        void Read(BleAdapterMessageCallback callback);
}
