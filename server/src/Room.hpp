#pragma once

#include "PlayerInfo.hpp"
#include "UDPSocket.hpp"

#include "../../protobuf/src/pnet.pb.h"

#include <vector>
#include <memory>

class Room
{
public:
    Room(uint16_t port, int maxPlayerCount = 4, size_t messageBufferSize = 1024);

    bool is_full() const;
    uint32_t address() const;
    uint16_t port() const;

    void serializeToRoomInfo(pnet::RoomInfo& roomInfo) const;

public:
    void seed();

private:
    const int m_maxPlayerCount;
    int m_playerCount;
    std::vector<PlayerInfo> m_playerList;

    int m_ownerID;

    const int m_bufferSize;
    const std::unique_ptr<uint8_t[]> m_buffer;

    UDPSocket m_socket;
};