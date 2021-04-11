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

    int saSize = sizeof(saServer);

    char buf[128];
    uint8_t buffer[128];

    while (true)
    {
        int len = scanf("%s", buf);
        printf("Read %d\n", len);
        
        int sentLen = sendto(socketFd, buf, strlen(buf), 0, (sockaddr*)&saServer, saSize);

        printf("Sent %d\n", sentLen);

        pnet::QueryRoomInfoRequest request{};

        request.set_empty_only(true);
        int requestSize = request.ByteSizeLong();
        request.SerializeToArray(buffer, requestSize);

        sentLen = sendto(socketFd, buffer, requestSize, 0, (sockaddr*)&saServer, saSize);

        printf("Sent %d\n", sentLen);
    }
    
}