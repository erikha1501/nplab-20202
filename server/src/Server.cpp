#include "Server.hpp"

#include <stdio.h>

Server::Server(uint16_t port, int maxRoomCount, size_t messageBufferSize)
    : m_socket(port, false), m_roomList(maxRoomCount), m_bufferSize(messageBufferSize),
      m_buffer(new uint8_t[messageBufferSize])
{
}

void Server::start()
{
    IPEndpoint sender;

    while (true)
    {
        ssize_t read = m_socket.recv(m_buffer.get(), m_bufferSize, sender);

        if (read == -1)
        {
            break;
        }

        if (read >= m_bufferSize)
        {
            continue;
        }

        printf("Received %ld byte packet: %s:%d\n", read, sender.addressAsStr(), sender.port());
        handleMessage(sender, m_buffer.get(), read);
    }
}

void Server::sendMessage(const IPEndpoint& receiver, MessageType type, const google::protobuf::Message& message)
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

void Server::handleMessage(const IPEndpoint& sender, const uint8_t* buffer, size_t size)
{
    // Read message type
    MessageType mesgType = (MessageType)buffer[0];
    size_t messageSize = size - 1;
    buffer += 1;

    switch (mesgType)
    {
    case MessageType::QueryRoomInfoRequest: {
        pnet::QueryRoomInfoRequest request;
        bool parseResult = request.ParseFromArray(buffer, messageSize);
        assert(parseResult);
        onQueryRoomInfo(sender, request);
        break;
    }
    case MessageType::CreateRoomRequest: {
        pnet::CreateRoomRequest request;
        bool parseResult = request.ParseFromArray(buffer, messageSize);
        assert(parseResult);
        onCreateRoom(sender, request);
        break;
    }
    case MessageType::JoinRoomRequest: {
        pnet::JoinRoomRequest request;
        bool parseResult = request.ParseFromArray(buffer, messageSize);
        assert(parseResult);
        onJoinRoom(sender, request);
        break;
    }
    case MessageType::LeaveRoomRequest: {
        pnet::LeaveRoomRequest request;
        bool parseResult = request.ParseFromArray(buffer, messageSize);
        assert(parseResult);
        onLeaveRoom(sender, request);
        break;
    }

    default:
        break;
    }
}

void Server::onQueryRoomInfo(const IPEndpoint& sender, const pnet::QueryRoomInfoRequest& request)
{
    pnet::QueryRoomInfoResponse response;

    for (const auto& [id, room] : m_roomList)
    {
        if (request.not_full() && room.is_full())
        {
            continue;
        }

        pnet::RoomInfo* roomInfo = response.add_rooms();
        roomInfo->set_id(id);
        room.serializeToRoomInfo(*roomInfo);
    }

    sendMessage(sender, MessageType::QueryRoomInfoResponse, response);
}

void Server::onCreateRoom(const IPEndpoint& sender, const pnet::CreateRoomRequest& request)
{
    pnet::CreateRoomResponse createRoomResponse{};

    const auto [roomIter, addRoomResult] = m_roomList.add(Room());
    
    // Fail to create new room
    if (!addRoomResult)
    {
        createRoomResponse.set_success(false);
        sendMessage(sender, MessageType::CreateRoomResponse, createRoomResponse);
        return;
    }

    auto& [roomID, newRoom] = *roomIter;

    Player player{request.client_info().name(), sender};
    int clientID = newRoom.addPlayer(std::move(player));
    
    // Fail to add player
    if (clientID < 0)
    {
        createRoomResponse.set_success(false);
        sendMessage(sender, MessageType::CreateRoomResponse, createRoomResponse);
        return;
    }

    // Successfully create a new room
    createRoomResponse.set_success(true);
    pnet::ClientIdentity* clientIdentity = createRoomResponse.mutable_assigned_identity();
    clientIdentity->set_room_id(roomID);
    clientIdentity->set_client_id(clientID);

    sendMessage(sender, MessageType::CreateRoomResponse, createRoomResponse);

    // Broadcast RoomInfoChanged message
    pnet::RoomInfoChanged roomInfoChangedMessage{};
    newRoom.serializeToRoomInfo(*roomInfoChangedMessage.mutable_new_room_info());

    for (const auto& [playerID, player] : newRoom)
    {
        sendMessage(player.endpoint(), MessageType::RoomInfoChanged, roomInfoChangedMessage);
    }
}

void Server::onJoinRoom(const IPEndpoint& sender, const pnet::JoinRoomRequest& request)
{
    pnet::JoinRoomResponse joinRoomResponse{};

    size_t roomID = request.room_id();
    Room* room = m_roomList.get(roomID);

    // Cannot find room with the given ID.
    if (room == nullptr)
    {
        joinRoomResponse.set_success(false);
        sendMessage(sender, MessageType::JoinRoomResponse, joinRoomResponse);
        return;
    }

    // Check if player is already in the room
    if (room->findPlayer(sender) != nullptr)
    {
        joinRoomResponse.set_success(false);
        sendMessage(sender, MessageType::JoinRoomResponse, joinRoomResponse);
        return;
    }

    // Try to add player
    Player player{request.client_info().name(), sender};
    int clientID = room->addPlayer(std::move(player));

    // Fail to add player
    if (clientID < 0)
    {
        joinRoomResponse.set_success(false);
        sendMessage(sender, MessageType::JoinRoomResponse, joinRoomResponse);
        return;
    }

    // Successfully add player
    joinRoomResponse.set_success(true);
    pnet::ClientIdentity* clientIdentity = joinRoomResponse.mutable_assigned_identity();
    clientIdentity->set_room_id(roomID);
    clientIdentity->set_client_id(clientID);

    sendMessage(sender, MessageType::JoinRoomResponse, joinRoomResponse);

    // Broadcast RoomInfoChanged message
    pnet::RoomInfoChanged roomInfoChangedMessage{};
    room->serializeToRoomInfo(*roomInfoChangedMessage.mutable_new_room_info());

    for (const auto& [playerID, player] : *room)
    {
        sendMessage(player.endpoint(), MessageType::RoomInfoChanged, roomInfoChangedMessage);
    }
}

void Server::onLeaveRoom(const IPEndpoint& sender, const pnet::LeaveRoomRequest& request)
{
    pnet::LeaveRoomResponse leaveRoomResponse{};

    size_t roomID = request.client_identity().room_id();
    Room* room = m_roomList.get(roomID);

    // Cannot find room with the given ID.
    if (room == nullptr)
    {
        leaveRoomResponse.set_success(false);
        sendMessage(sender, MessageType::LeaveRoomResponse, leaveRoomResponse);
        return;
    }
    
    uint clientID = request.client_identity().client_id();
    const Player* player = room->findPlayer(clientID);

    // Cannot find player with the given ID.
    if (player == nullptr)
    {
        leaveRoomResponse.set_success(false);
        sendMessage(sender, MessageType::LeaveRoomResponse, leaveRoomResponse);
        return;
    }

    // The endpoints dont match
    if (player->endpoint() != sender)
    {
        leaveRoomResponse.set_success(false);
        sendMessage(sender, MessageType::LeaveRoomResponse, leaveRoomResponse);
        return;
    }
    
    room->removePlayer(clientID);

    // Successfully remove player
    leaveRoomResponse.set_success(true);
    sendMessage(sender, MessageType::LeaveRoomResponse, leaveRoomResponse);

    // Broadcast RoomInfoChanged message
    pnet::RoomInfoChanged roomInfoChangedMessage{};
    room->serializeToRoomInfo(*roomInfoChangedMessage.mutable_new_room_info());

    for (const auto& [playerID, player] : *room)
    {
        sendMessage(player.endpoint(), MessageType::RoomInfoChanged, roomInfoChangedMessage);
    }

    // The last player has left the room
    if (room->size() == 0)
    {
        m_roomList.remove(roomID);
    }
}