#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string.h>

#define DEFAULT_BUFLEN 4096
char recvbuffer[DEFAULT_BUFLEN];

int main()
{
    WSADATA wsaData;
    int result;
    WORD version = MAKEWORD(2, 2);
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

    memset(&saServer, 0, sizeof(saServer));
    saServer.sin_family = AF_INET;
    saServer.sin_port = htons(8080);

    int ipResult = InetPton(saServer.sin_family, "127.0.0.1", &saServer.sin_addr);

    if (ipResult != 1)
    {
        printf("InetPton failed\n");
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }

    // Setup the TCP listening socket
    int resolve = bind(sockfd, (const SOCKADDR *)&saServer, sizeof(saServer));

    if (resolve == SOCKET_ERROR)
    {
        printf("bind failed with error: %d\n", WSAGetLastError());
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }

    // listening for an incoming connection
    int listenResult = listen(sockfd, SOMAXCONN);

    if (listenResult == SOCKET_ERROR)
    {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }

    // accepting connections from clients
    struct sockaddr_in clientInfo;
    memset(&clientInfo, 0, sizeof(clientInfo));
    int clientInfoSize = sizeof(clientInfo);
    SOCKET clientSocket = accept(sockfd, (struct sockaddr *)&clientInfo,
                                 &clientInfoSize);

    if (clientSocket == INVALID_SOCKET)
    {
        printf("accept failed: %d\n", WSAGetLastError());
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }

    // receive and send data on a socket
    int recieved = recv(clientSocket, recvbuffer, DEFAULT_BUFLEN - 1, 0);
    if (recieved > 0)
    {
        printf("Bytes received: %d\n", recieved);
        recvbuffer[recieved] = '\0';
        printf("%s\n", recvbuffer);
    }
    else if (recieved == 0)
    {
        printf("Connection closing...\n");
    }
    else
    {
        printf("recv failed: %d\n", WSAGetLastError());
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }
}