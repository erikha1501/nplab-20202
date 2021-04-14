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

        room.serializeToRoomInfo(*roomInfo);
    }

    sendMessage(sender, MessageType::QueryRoomInfoResponse, response);
}

void Server::onCreateRoom(const IPEndpoint& sender, const pnet::CreateRoomRequest& request)
{
    const auto [roomIter, addRoomResult] = m_roomList.add(Room());
    assert(addRoomResult);

    Player player{request.client_info().name(), sender};
    int clientId = roomIter->second.addPlayer(std::move(player));
    assert(clientId >= 0);

    pnet::CreateRoomResponse response{};

    response.set_success(true);
    pnet::ClientIdentity* clientIdentity = response.mutable_assigned_identity();
    clientIdentity->set_room_id(roomIter->first);
    clientIdentity->set_client_id(clientId);

    sendMessage(sender, MessageType::CreateRoomResponse, response);
}

void Server::onJoinRoom(const IPEndpoint& sender, const pnet::JoinRoomRequest& request)
{
}

void Server::onLeaveRoom(const IPEndpoint& sender, const pnet::LeaveRoomRequest& request)
{
}