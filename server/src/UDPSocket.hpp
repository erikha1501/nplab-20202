#pragma once

#include "IPEndpoint.hpp"

#include <arpa/inet.h>
#include <sys/socket.h>

class UDPSocket
{
public:
    UDPSocket(uint16_t port, bool blocking);
    UDPSocket(const UDPSocket&) = delete;
    ~UDPSocket();

    uint16_t bindedPort() const;

    ssize_t recv(uint8_t* buffer, size_t size, IPEndpoint& sender);
    ssize_t send(const uint8_t* buffer, size_t size, const IPEndpoint& receiver);
private:
    uint16_t m_bindedPort;
    int m_socketFd;
};