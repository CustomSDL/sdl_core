#include "transport_manager/bluetooth_le/ble_client.h"

#include <thread>
#include "utils/logger.h"

SDL_CREATE_LOG_VARIABLE("Ble_Client")

namespace{
    const char* socket_name =  "./localBleReader";

    char buffer[256];
    bool thread_stop = false;

    void writer_func(int socket_id)
    {
        while(!thread_stop)
        {
            sleep(5);
            sprintf(buffer, "qwerty");
            int n = write(socket_id, buffer, 8);
            SDL_LOG_INFO("@@@BLE_Write " << n);
        }
    }
}

void BleClient::Init()
{
    SDL_LOG_TRACE("enter");
    connected = false;

    struct sockaddr_un addr;
    socklen_t len;
    addr.sun_family = AF_LOCAL;
    addr.sun_path[0] = '\0';
    strcpy(&addr.sun_path[1], socket_name );
    len = offsetof(struct sockaddr_un, sun_path) + 1 + strlen(&addr.sun_path[1]);
    int err;

    socket_id = socket(PF_LOCAL, SOCK_STREAM, 0);
    if (socket_id < 0) {
        err = errno;
        SDL_LOG_INFO("Cannot open socket: " << strerror(err));
        errno = err;
        return;
    }

    if (connect(socket_id, (struct sockaddr *) &addr, len) < 0) {
        err = errno;
        SDL_LOG_INFO("connect() failed: " <<
             strerror(err));
        close(socket_id);
        errno = err;
        return;
    }

    connected = true;
    SDL_LOG_TRACE("exit");
}

void BleClient::Run()
{
    SDL_LOG_TRACE("enter");
    if(connected){
        writer_func(socket_id);
    }
    SDL_LOG_TRACE("exit");
}

BleClient::~BleClient()
{
    if(connected)
    {
        close(socket_id);
    }
}