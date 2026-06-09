#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#define DEFAULT_BUF_LEN 4096

char recvBuffer[DEFAULT_BUF_LEN];
char sendBuffer[DEFAULT_BUF_LEN];

int main(void)
{
    // Initialize Winsock
    WSADATA wsaData;
    WORD version = MAKEWORD(2, 2);

    int result = WSAStartup(version, &wsaData);

    if (result != 0)
    {
        printf("WSAStartup failed: %d\n", result);
        return 1;
    }

    // Create listening socket
    SOCKET sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd == INVALID_SOCKET)
    {
        printf("socket failed: %ld\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // Configure server address
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);

    int ipResult = InetPton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

    if (ipResult != 1)
    {
        printf("InetPton failed\n");
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }

    // Bind socket to address
    int bindResult =
        bind(sockfd, (const SOCKADDR *)&serverAddr, sizeof(serverAddr));

    if (bindResult == SOCKET_ERROR)
    {
        printf("bind failed: %d\n", WSAGetLastError());
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }

    // Start listening
    int listenResult = listen(sockfd, SOMAXCONN);

    if (listenResult == SOCKET_ERROR)
    {
        printf("listen failed: %d\n", WSAGetLastError());
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }

    printf("Server listening on http://127.0.0.1:8080\n");

    // Accept a client connection
    struct sockaddr_in clientAddr;
    memset(&clientAddr, 0, sizeof(clientAddr));

    int clientAddrSize = sizeof(clientAddr);

    SOCKET clientSocket =
        accept(sockfd, (struct sockaddr *)&clientAddr, &clientAddrSize);

    if (clientSocket == INVALID_SOCKET)
    {
        printf("accept failed: %d\n", WSAGetLastError());
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }

    // Receive request
    int received =
        recv(clientSocket, recvBuffer, DEFAULT_BUF_LEN - 1, 0);

    if (received > 0)
    {
        recvBuffer[received] = '\0';

        printf("Bytes received: %d\n\n", received);
        printf("%s\n", recvBuffer);
    }
    else if (received == 0)
    {
        printf("Connection closing...\n");
    }
    else
    {
        printf("recv failed: %d\n", WSAGetLastError());

        closesocket(clientSocket);
        closesocket(sockfd);
        WSACleanup();

        return 1;
    }

    // Send HTTP response
    const char *body = "Hello, World!";

    snprintf(
        sendBuffer,
        sizeof(sendBuffer),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: %zu\r\n"
        "\r\n"
        "%s", strlen(body), body);

    int bytesSent =
        send(clientSocket, sendBuffer, strlen(sendBuffer), 0);

    if (bytesSent == SOCKET_ERROR)
    {
        printf("send failed: %d\n", WSAGetLastError());

        closesocket(clientSocket);
        closesocket(sockfd);
        WSACleanup();

        return 1;
    }

    printf("Bytes sent: %d\n", bytesSent);

    closesocket(clientSocket);
    closesocket(sockfd);
    WSACleanup();

    return 0;
}
