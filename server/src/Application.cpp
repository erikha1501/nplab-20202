#include "Server.hpp"
#include "game/GameServer.hpp"

#include "game/map/MapData.hpp"

#include "stdio.h"

int main()
{
    // constexpr uint16_t port = 6969;
    // constexpr int maxRoomCount = 4;

    // Server server(port, maxRoomCount);

    // server.start();

    game::GameServer gameServer{0, 6970};
    gameServer.addPlayer(0, IPEndpoint());
    gameServer.start();

    char choice;
    while (true)
    {
        scanf(" %c", &choice);

        if (choice == 'q')
        {
            printf("Stopping game instace\n");
            gameServer.stop();
            break;
        }
    }

    gameServer.wait();

    // game::map::MapData map{16, 16};
    // map.clear();
    // for (int y = 1; y < map.height() - 1; y++)
    // {
    //     map.at(10, y) = game::map::TileType::Wall;
    // }

    // for (int y = 0; y < map.height(); y++)
    // {
    //     for (int x = 0; x < map.width(); x++)
    //     {
    //         printf("%d ", (int)map.at(x, y));
    //     }
        
    //     printf("\n");
    // }

    // game::map::MapData::saveToFile(map, game::map::MapData::s_defaultMapFileName);
}