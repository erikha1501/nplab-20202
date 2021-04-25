#include "GameServer.hpp"

#include "../MessageType.hpp"

#include <pnet.pb.h>

namespace game
{

GameServer::GameServer(size_t roomID, uint16_t port)
    : m_roomID(roomID), m_playerInfoList{}, m_gameWorld{}, m_socket(port, false), m_buffer(new uint8_t[s_bufferSize])
{
    // TODO: consider encapsulating PlayerInfo into a class
    for (auto& player : m_playerInfoList)
    {
        player.id = -1;
    }
}

bool GameServer::addPlayer(uint id, const IPEndpoint& endpoint)
{
    if (id >= g_maxPlayerCount)
    {
        return false;
    }
    
    m_playerInfoList[id].id = id;
    m_playerInfoList[id].endpoint = endpoint;
    m_playerInfoList[id].connectionState = PlayerInfo::ConnectionState::Connected;

    m_gameWorld.addPlayer(id);

    return true;
}

void GameServer::start()
{
    m_gameWorld.initialize();

    m_running = true;
    m_thread = std::thread{&GameServer::startInternal, this};
}
void GameServer::stop()
{
    m_running = false;
}

void GameServer::wait()
{
    if (m_thread.joinable())
    {
        m_thread.join();
    }
}

void GameServer::startInternal()
{
    constexpr time_duration frameDeltaTime{1.0f / 40.0f};   // 40 fps
    constexpr time_duration physicDeltaTime{1.0f / 120.0f};  // 120 tps
    hrc_time_point currentTime = hrclock::now();

    time_duration physicTimeBudget{0.0f};

    while (m_running)
    {
        // Calculate time between frames
        hrc_time_point newTime = hrclock::now();
        time_duration frameTime = std::chrono::duration_cast<time_duration>(newTime - currentTime);
        currentTime = newTime;
        physicTimeBudget += frameTime;

        pollPlayerInput();

        // Update player input
        m_gameWorld.applyPlayerInput();

        // Update game state
        while (physicTimeBudget >= physicDeltaTime)
        {
            m_gameWorld.step(physicDeltaTime);
            physicTimeBudget -= physicDeltaTime;
        }

        sendGameState();

        std::this_thread::sleep_for(frameDeltaTime - (hrclock::now() - currentTime));
        printf("\rFrame time: %8.7f ms    ", frameTime.count() * 1000);
        fflush(stdout);
    }
}

bool GameServer::validatePlayerIdentity(size_t roomID, uint clientID, const IPEndpoint& endpoint)
{
    if (roomID != m_roomID)
    {
        return false;
    }

    if (clientID >= g_maxPlayerCount)
    {
        return false;
    }

    return m_playerInfoList[clientID].endpoint == endpoint;
}

void GameServer::pollPlayerInput()
{
    IPEndpoint sender;
    while (true)
    {
        uint8_t* buffer = m_buffer.get();
        int read = m_socket.recv(buffer, s_bufferSize, sender);

        if (read == -1)
        {
            if (errno != EWOULDBLOCK)
            {
                // TODO: Log
                m_running = false;
            }

            break;
        }
        MessageType messageType = (MessageType)buffer[0];
        buffer += 1;
        size_t messageSize = read - 1;

        switch (messageType)
        {
        case MessageType::PlayerInput: {
            pnet::PlayerInput playerInputMessage;
            playerInputMessage.ParseFromArray(buffer, messageSize);

            const pnet::ClientIdentity& identity = playerInputMessage.player_identity();
            uint clientID = identity.client_id();
            if (!validatePlayerIdentity(identity.room_id(), clientID, sender))
            {
                // TODO: Log
                break;
            }

            PlayerEntity::InputData& playerInput = m_gameWorld.playerInput(clientID);
            playerInput.direction.Set(playerInputMessage.direction_x(), playerInputMessage.direction_y());
            playerInput.placeBomb = playerInputMessage.bomb_placed();

            break;
        }

        default:
            // TODO: remove this
            addPlayer(0, sender);
            break;
        }
    }
}

void GameServer::sendGameState()
{
    pnet::GameState gameState;

    for (uint i = 0; i < m_playerInfoList.size(); i++)
    {
        PlayerInfo& player = m_playerInfoList[i];
        if (player.id < 0 || player.connectionState == PlayerInfo::ConnectionState::Disconnected)
        {
            continue;
        }

        pnet::PlayerState* playerState = gameState.add_player_states();

        const PlayerEntity& playerEntity = m_gameWorld.playerEntity(i);
        playerState->set_client_id(i);
        playerState->set_position_x(playerEntity.body->GetPosition().x);
        playerState->set_position_y(playerEntity.body->GetPosition().y);
        playerState->set_direction_x(playerEntity.body->GetLinearVelocity().x);
        playerState->set_direction_y(playerEntity.body->GetLinearVelocity().y);
    }

    for (auto& player : m_playerInfoList)
    {
        if (player.id < 0 || player.connectionState == PlayerInfo::ConnectionState::Disconnected)
        {
            continue;
        }

        sendMessage(player.endpoint, MessageType::GameState, gameState);
    }
}

void GameServer::sendMessage(const IPEndpoint& receiver, MessageType type, const google::protobuf::Message& message)
{
    uint8_t* buffer = m_buffer.get();

    // Write message type
    buffer[0] = (uint8_t)type;

    // Write message
    size_t messageSize = message.ByteSizeLong();
    bool ret = message.SerializeToArray(buffer + 1, messageSize);
    assert(ret);

    m_socket.send(buffer, messageSize + 1, receiver);
}

} // namespace game
