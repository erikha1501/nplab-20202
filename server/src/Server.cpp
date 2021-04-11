#include "Server.hpp"

#include "MessageType.hpp"

#include <stdio.h>

Server::Server(uint16_t port, int maxRoomCount, size_t messageBufferSize) :
    m_socket(port, false), m_maxRoomCount(maxRoomCount),
    m_bufferSize(messageBufferSize), m_buffer(new uint8_t[messageBufferSize])
{
}

void seed(std::vector<Room>& roomList)
{
    roomList.emplace_back(6970);
}

void Server::start()
{
    seed(m_roomList);

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

void Server::sendMessage(const IPEndpoint& receiver, const google::protobuf::Message& message)
{
    size_t messageSize = message.ByteSizeLong();
    bool ret = message.SerializeToArray(m_buffer.get(), messageSize);

    assert(ret);

    m_socket.send(m_buffer.get(), messageSize, receiver);
}

void Server::handleMessage(const IPEndpoint& sender, const uint8_t* buffer, size_t size)
{
    MessageType mesgType = (MessageType)buffer[0];
    size_t messageSize = size - 1;
    buffer += 1;

    switch (mesgType)
    {
    case MessageType::QueryRoomInfoRequest: 
    {
        pnet::QueryRoomInfoRequest request;
        bool ret = request.ParseFromArray(buffer, messageSize);
        assert(ret);
        onQueryRoomInfo(sender, request);
        break;
    }
    
    default:
        break;
    }
}

void Server::onQueryRoomInfo(const IPEndpoint& sender, const pnet::QueryRoomInfoRequest& request)
{
    pnet::QueryRoomInfoResponse response;

    for (auto& room : m_roomList)
    {
        if (request.not_full() && room.is_full())
        {
            continue;
        }

        pnet::RoomInfo* roomInfo = response.add_rooms();

        room.serializeToRoomInfo(*roomInfo);
    }

    sendMessage(sender, response);
}