#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>

int main()
{
    WSADATA wsaData;
    int result;
    int version = MAKEWORD(2, 2);
    result = WSAStartup(version, &wsaData);

    if (result != 0)
    {
        printf("WSAStartup failed with error: %d\n", result);

        return 1;
    }
    // Create socket
    SOCKET sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd == INVALID_SOCKET)
    {
        printf("Error at socket(): %ld\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    struct sockaddr_in saServer;

    saServer.sin_family = AF_INET;
    saServer.sin_port = htons(8080);

    InetPton(saServer.sin_family, "127.0.0.1", &saServer.sin_addr);
}