#pragma once

#include "IPEndpoint.hpp"
#include "Player.hpp"
#include "util/ReusableIdList.hpp"

#include "../../protobuf/src/pnet.pb.h"

#include <vector>

class Room
{
public:
    using iterator = ReuseableIdList<Player>::iterator;
    using const_iterator = ReuseableIdList<Player>::const_iterator;

    Room();

    bool is_full() const
    {
        return m_playerList.size() >= s_maxPlayerCount;
    }

    int ownerID() const
    {
        return m_ownerID;
    }

    int addPlayer(Player player);
    bool removePlayer(uint id);

    const Player* findPlayer(uint id) const;
    const Player* findPlayer(const IPEndpoint& endpoint) const;

    size_t size() const
    {
        return m_playerList.size();
    }

    const_iterator begin() const
    {
        return m_playerList.begin();
    }
    const_iterator end() const
    {
        return m_playerList.end();
    }

    void serializeToRoomInfo(pnet::RoomInfo& roomInfo) const;

private:
    static constexpr uint s_maxPlayerCount = 4;

    ReuseableIdList<Player> m_playerList;

    int m_ownerID;
};