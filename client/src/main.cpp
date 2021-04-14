#include "../../protobuf/src/pnet.pb.h"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <stdio.h>
#include <string.h>

int main()
{
    int socketFd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    sockaddr_in saServer{};
    saServer.sin_family = AF_INET;
    saServer.sin_port = htons(6969);
    saServer.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    sockaddr_in anon{};
    anon.sin_family = AF_INET;

    socklen_t saSize = sizeof(saServer);

    char buf[128];
    uint8_t buffer[128];

    while (true)
    {
        int len = scanf("%s", buf);
        printf("Read %d\n", len);

        pnet::QueryRoomInfoRequest request{};

        request.set_not_full(true);
        int requestSize = request.ByteSizeLong();

        buffer[0] = 0;
        request.SerializeToArray(buffer + 1, requestSize);

        int sentLen = sendto(socketFd, buffer, requestSize + 1, 0, (sockaddr*)&saServer, saSize);
        printf("Sent %d\n", sentLen);

        int readLen = recvfrom(socketFd, buffer, 128, 0, (sockaddr*)&anon, &saSize);

        assert(readLen > 0);
        assert(buffer[0] == 1);

        pnet::QueryRoomInfoResponse response{};
        response.ParseFromArray(buffer + 1, readLen - 1);

        for (auto& roomInfo : response.rooms())
        {
            printf("Room %d\n", roomInfo.port());

            for (auto& clientInfo : roomInfo.clients())
            {
                printf("%s\n", clientInfo.name().c_str());
            }
            
            printf("\n");
        }
        
    }
    
}