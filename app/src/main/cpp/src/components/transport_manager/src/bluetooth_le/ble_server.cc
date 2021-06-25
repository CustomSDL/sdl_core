#include "transport_manager/bluetooth_le/ble_server.h"
#include <thread>
#include "utils/logger.h"

SDL_CREATE_LOG_VARIABLE("Ble_Server")

namespace{
    const char* socket_name =  "./localBleWriter";

    char buffer[256];
    bool thread_stop = false;

    void reader_func(int socket_id) {
        while (!thread_stop) {
            int n = read(socket_id, buffer, 255);
            if (n > 0) {
                SDL_LOG_INFO("@@@BLE_ " << buffer);
            }
        }
    }
}

void BleServer::Init()
{
    SDL_LOG_TRACE("enter");
    connected = false;

    struct sockaddr_un server_sockaddr;
    struct sockaddr_un client_sockaddr;
    socklen_t len;
    server_sockaddr.sun_family = AF_LOCAL;
    server_sockaddr.sun_path[0] = '\0';
    strcpy(&server_sockaddr.sun_path[1], socket_name );
    len = offsetof(struct sockaddr_un, sun_path) + 1 + strlen(&server_sockaddr.sun_path[1]);
    int err;

    server_sock = socket(PF_LOCAL, SOCK_STREAM, 0);
    if (server_sock < 0) {
        err = errno;
        SDL_LOG_INFO("Cannot open socket: " << strerror(err));
        errno = err;
        return;
    }

    unlink(socket_name);
    int rc = bind(server_sock, (struct sockaddr *) &server_sockaddr, len);
    if (rc < 0){
        err = errno;
        SDL_LOG_ERROR("BIND ERROR: " << strerror(err));
        close(server_sock);
        return;
    }

    int backlog = 1;
    rc = listen(server_sock, backlog);
    if (rc < 0){
        err = errno;
        SDL_LOG_ERROR("LISTEN ERROR: " << strerror(err));
        close(server_sock);
        return;
    }

    SDL_LOG_INFO("socket listening...");

    client_sock = accept(server_sock, (struct sockaddr *) &client_sockaddr, &len);
    if (client_sock < 0){
        err = errno;
        SDL_LOG_ERROR("ACCEPT ERROR: " << strerror(err));
        close(server_sock);
        close(client_sock);
        return;
    }

    connected = true;
    SDL_LOG_TRACE("exit");
}

void BleServer::Run()
{
    SDL_LOG_TRACE("enter");
    if(connected){
        reader_func(client_sock);
    }
    SDL_LOG_TRACE("exit");
}

BleServer::~BleServer(){
    if(connected){
        close(client_sock);
        close(server_sock);
    }
}
