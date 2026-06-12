#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#define DEFAULT_BUF_LEN 4096

char recvBuffer[DEFAULT_BUF_LEN];
char sendBuffer[DEFAULT_BUF_LEN];

char method[16];
char path[256];

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
    while (1)
    {

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
            printf("%s\n", recvBuffer);
        }
        else if (received == 0)
        {
            printf("Connection closing...\n");
            closesocket(clientSocket);
            continue;
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
        char *filename = "index.html";
        char *contentType = "text/html";
        if (sscanf(recvBuffer, "%15s %255s", method, path) != 2)
        {
            closesocket(clientSocket);
            continue;
        }
        if (strcmp(path, "/") != 0)
        {
            filename = path + 1;
        }
        if (strstr(filename, ".css") != NULL)
        {
            contentType = "text/css";
        }
        else if (strstr(filename, ".js") != NULL)
        {
            contentType = "application/javascript";
        }

        FILE *htmlFile = fopen(filename, "rb");
        if (htmlFile == NULL)
        {
            snprintf(
                sendBuffer,
                sizeof(sendBuffer),
                "HTTP/1.1 404 Not Found\r\n"
                "Connection: close\r\n"
                "Content-Length: 0\r\n"
                "\r\n");
            send(clientSocket, sendBuffer, strlen(sendBuffer), 0);
            closesocket(clientSocket);
            continue;
        }
        fseek(htmlFile, 0, SEEK_END);
        long fileSize = ftell(htmlFile);
        if (fileSize == -1)
        {
            printf("ftell failed\n");
            fclose(htmlFile);
            closesocket(clientSocket);
            continue;
        }
        fseek(htmlFile, 0, SEEK_SET);
        char *htmlBuffer = malloc((size_t)fileSize + 1);
        if (htmlBuffer == NULL)
        {
            printf("Memory Allocation failed!\n");
            closesocket(clientSocket);
            free(htmlBuffer);
            fclose(htmlFile);
            return 1;
        }
        size_t readBytes = fread(htmlBuffer, 1, fileSize, htmlFile);

        htmlBuffer[fileSize] = '\0';
        if (readBytes == fileSize)
        {

            snprintf(
                sendBuffer,
                sizeof(sendBuffer),
                "HTTP/1.1 200 OK\r\n"
                "Connection: close\r\n"
                "Content-Type: %s\r\n"
                "Content-Length: %zu\r\n"
                "\r\n",
                contentType,
                fileSize);
        }
        else
        {
            printf("Couldnt read the file\n");
            closesocket(clientSocket);
            free(htmlBuffer);
            fclose(htmlFile);
            return 1;
        }
        int header_bytes_sent = send(clientSocket, sendBuffer, strlen(sendBuffer), 0);
        int body_bytes_sent = send(clientSocket, htmlBuffer, readBytes, 0);
        if (body_bytes_sent == SOCKET_ERROR || header_bytes_sent == SOCKET_ERROR)
        {
            printf("Sending http response failed: %d\n", WSAGetLastError());
            fclose(htmlFile);
            closesocket(clientSocket);
            free(htmlBuffer);
            continue;
        }
        free(htmlBuffer);
        fclose(htmlFile);
        closesocket(clientSocket);
    }

    closesocket(sockfd);
    WSACleanup();
    return 0;
}
