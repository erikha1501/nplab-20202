#include "Server.hpp"

int main()
{
    constexpr uint16_t port = 6969;
    constexpr int maxRoomCount = 4;

    Server server(port, maxRoomCount);

    server.start();
}