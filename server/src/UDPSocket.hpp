#pragma once

#include "IPEndpoint.hpp"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

class UDPSocket
{
public:
    UDPSocket(const UDPSocket&) = delete;
    UDPSocket& operator=(const UDPSocket&) = delete;

    UDPSocket(UDPSocket&&) = default;
    UDPSocket& operator=(UDPSocket&&) = default;

    UDPSocket(bool blocking) : m_bindedPort(0), m_socketFd(-1)
    {
        int socketType = SOCK_DGRAM | (blocking ? 0 : SOCK_NONBLOCK);
        m_socketFd = socket(AF_INET, socketType, IPPROTO_UDP);

        if (m_socketFd == -1)
        {
            throw;
        }
    }

    UDPSocket(uint16_t bindedPort, bool blocking) : UDPSocket(blocking)
    {
        m_bindedPort = bindedPort;

        sockaddr_in sockAddr{};
        sockAddr.sin_family = AF_INET;
        sockAddr.sin_port = htons(bindedPort);
        sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);

        int ret = bind(m_socketFd, (sockaddr*)&sockAddr, sizeof(sockAddr));

        if (ret == -1)
        {
            throw;
        }
    }

    ~UDPSocket()
    {
        if (m_socketFd >= 0)
        {
            close(m_socketFd);
            m_socketFd = -1;
        }
    }

    int socketFd() const
    {
        return m_socketFd;
    }

    uint16_t bindedPort() const
    {
        return m_bindedPort;
    }

    ssize_t recv(uint8_t* buffer, size_t size, IPEndpoint& sender)
    {
        socklen_t socketSize = sender.size();
        return recvfrom(m_socketFd, buffer, size, 0, sender.get(), &socketSize);
    }

    ssize_t send(const uint8_t* buffer, size_t size, const IPEndpoint& receiver)
    {
        return sendto(m_socketFd, buffer, size, 0, receiver.getConst(), receiver.size());
    }

private:
    uint16_t m_bindedPort;
    int m_socketFd;
};