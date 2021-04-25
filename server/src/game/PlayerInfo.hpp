#pragma once

#include "core.hpp"

#include "../IPEndpoint.hpp"

namespace game
{

struct PlayerInfo
{
    enum class ConnectionState
    {
        Disconnected = 0,
        Connecting = 1,
        Connected = 2
    };

    int id;

    time_duration timeSinceLastMessage;
    ConnectionState connectionState;
    IPEndpoint endpoint;
};

} // namespace game
