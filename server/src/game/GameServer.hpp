#pragma once

#include "core.hpp"

#include "PlayerInfo.hpp"
#include "GameWorld.hpp"
#include "../MessageType.hpp"
#include "../UDPSocket.hpp"

#include <pnet.pb.h>

#include <array>
#include <thread>

namespace game
{

class GameServer
{
public:
    GameServer(size_t roomID, uint16_t port);

    bool addPlayer(uint id, const IPEndpoint& endpoint);

    void start();
    void stop();
    void wait();

private:
    void startInternal();

    bool validatePlayerIdentity(size_t roomID, uint clientID, const IPEndpoint& endpoint);

    void pollPlayerInput();
    void sendGameState();

    void sendMessage(const IPEndpoint& receiver, MessageType type, const google::protobuf::Message& message);

private:
    size_t m_roomID;

    std::array<PlayerInfo, g_maxPlayerCount> m_playerInfoList;

    GameWorld m_gameWorld;

    bool m_running;
    std::thread m_thread;

    static constexpr uint s_bufferSize = 1024;
    std::unique_ptr<uint8_t[]> m_buffer;

    UDPSocket m_socket;
};

} // namespace game
