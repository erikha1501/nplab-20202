#pragma once

#include "Player.hpp"
#include "IPEndpoint.hpp"
#include "util/ReusableIdList.hpp"

#include "../../protobuf/src/pnet.pb.h"

#include <vector>

class Room
{
public:
    Room();

    bool is_full() const;
    int ownerID() const;

    int addPlayer(Player player);
    bool removePlayer(uint id);

    const Player* findPlayer(uint id) const;

    void serializeToRoomInfo(pnet::RoomInfo& roomInfo) const;

private:
    static constexpr uint s_maxPlayerCount = 4;
    
    ReuseableIdList<Player> m_playerList;

    int m_ownerID;
};