package org.luxoft.sdl_core;

public interface BleReader {
        void Connect(OnConnectCallback callback);
        void Disconnect();
        void Read(BleAdapterMessageCallback callback);
}
