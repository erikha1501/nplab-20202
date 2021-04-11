#pragma once

#include "Room.hpp"
#include "UDPSocket.hpp"

#include "../../protobuf/src/pnet.pb.h"

#include <vector>
#include <memory>

class Server
{
public:
    Server(uint16_t port = 6969, int maxRoomCount = 4, size_t messageBufferSize = 1024);

    void start();

private:
    void handleMessage(const IPEndpoint& sender, const uint8_t* buffer, size_t size);

    void sendMessage(const IPEndpoint& receiver, const google::protobuf::Message& message);

    void onQueryRoomInfo(const IPEndpoint& sender, const pnet::QueryRoomInfoRequest& request);
private:
    const int m_maxRoomCount;
    std::vector<Room> m_roomList;

    const int m_bufferSize;
    const std::unique_ptr<uint8_t[]> m_buffer;

    UDPSocket m_socket;
};