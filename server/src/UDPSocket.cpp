#include "UDPSocket.hpp"

#include <unistd.h>

UDPSocket::UDPSocket(uint16_t port, bool blocking) : m_bindedPort(port), m_socketFd(-1)
{
    int socketType = SOCK_DGRAM | (blocking ? SOCK_NONBLOCK : 0);
    m_socketFd = socket(AF_INET, socketType, IPPROTO_UDP);

    if (m_socketFd == -1)
    {
        throw;
    }

    sockaddr_in sockAddr{};
    sockAddr.sin_family = AF_INET;
    sockAddr.sin_port = htons(port);
    sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    int ret = bind(m_socketFd, (sockaddr*)&sockAddr, sizeof(sockAddr));

    if (ret == -1)
    {
        throw;
    }
}

UDPSocket::~UDPSocket()
{
    if (m_socketFd >= 0)
    {
        close(m_socketFd);
        m_socketFd = -1;
    }
}

uint16_t UDPSocket::bindedPort() const
{
    return m_bindedPort;
}

ssize_t UDPSocket::recv(uint8_t* buffer, size_t size, IPEndpoint& sender)
{
    socklen_t socketSize = sender.size();
    return recvfrom(m_socketFd, buffer, size, 0, sender.get(), &socketSize);
}

ssize_t UDPSocket::send(const uint8_t* buffer, size_t size, const IPEndpoint& receiver)
{
    return sendto(m_socketFd, buffer, size, 0, receiver.getConst(), receiver.size());
}
