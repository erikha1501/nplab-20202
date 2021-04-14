#pragma once

#include "Room.hpp"
#include "UDPSocket.hpp"
#include "MessageType.hpp"
#include "util/ReusableIdList.hpp"

#include "../../protobuf/src/pnet.pb.h"

#include <memory>
#include <vector>
#include <stack>

class Server
{
public:
    Server(uint16_t port = 6969, int maxRoomCount = 4, size_t messageBufferSize = 1024);

    void start();

private:
    void handleMessage(const IPEndpoint& sender, const uint8_t* buffer, size_t size);

    void sendMessage(const IPEndpoint& receiver, MessageType type, const google::protobuf::Message& message);

    void onQueryRoomInfo(const IPEndpoint& sender, const pnet::QueryRoomInfoRequest& request);
    void onCreateRoom(const IPEndpoint& sender, const pnet::CreateRoomRequest& request);
    void onJoinRoom(const IPEndpoint& sender, const pnet::JoinRoomRequest& request);
    void onLeaveRoom(const IPEndpoint& sender, const pnet::LeaveRoomRequest& request);

private:
    // TODO: refactor these 3 into a dedicated class
    ReuseableIdList<Room> m_roomList;

    int m_bufferSize;
    std::unique_ptr<uint8_t[]> m_buffer;

    UDPSocket m_socket;
};