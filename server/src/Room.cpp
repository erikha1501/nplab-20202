#include "Room.hpp"

Room::Room(uint16_t port, int maxPlayerCount, size_t messageBufferSize)
    : m_socket(port, true), m_maxPlayerCount(maxPlayerCount),
      m_bufferSize(messageBufferSize), m_buffer(new uint8_t[messageBufferSize])
{
    m_playerCount = 0;
    m_ownerID = -1;

    seed();
}

void Room::seed()
{
}

bool Room::is_full() const
{
    return m_playerCount >= m_maxPlayerCount;
}
uint32_t Room::address() const
{
    // Assuming room address is the same as server address.
    return 0;
}
uint16_t Room::port() const
{
    return m_socket.bindedPort();
}

void Room::serializeToRoomInfo(pnet::RoomInfo& roomInfo) const
{
    roomInfo.set_address(0);
    roomInfo.set_port(port());
    roomInfo.set_is_full(is_full());
    roomInfo.set_owner_id(m_ownerID);

    for (auto& player : m_playerList)
    {
        pnet::RoomInfo_ClientInfo* clientInfo = roomInfo.add_clients();
        clientInfo->set_name(player.name());
    }
}