#pragma once

enum class MessageType
{
    QueryRoomInfoRequest = 0,
    QueryRoomInfoResponse = 1,

    CreateRoomRequest = 2,
    CreateRoomResponse = 3,

    JoinRoomRequest = 4,
    JoinRoomResponse = 5,

    LeaveRoomRequest = 6,
    LeaveRoomResponse = 7
};