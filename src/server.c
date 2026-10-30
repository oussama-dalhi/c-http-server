#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    typedef SOCKET socket_t;
    typedef int addrlen_t;
    #define INVALID_SOCK INVALID_SOCKET
    #define SOCKET_ERROR_CODE SOCKET_ERROR
    #define close_socket closesocket
#else
    #include <errno.h>
    #include <unistd.h>
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <netinet/in.h>
    typedef int socket_t;
    typedef socklen_t addrlen_t;
    #define INVALID_SOCK (-1)
    #define SOCKET_ERROR_CODE (-1)
    #define INVALID_SOCK (-1)
    #define close_socket close
#endif

#define DEFAULT_BUF_LEN 4096

char recvBuffer[DEFAULT_BUF_LEN];
char sendBuffer[DEFAULT_BUF_LEN];

char method[16];
char path[256];

int main(void)
{
#ifdef _WIN32
    
    WSADATA wsaData;
    int result;
    result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if(result != 0)
    {
        printf("WSAStartup failed: %d\n", result);
        return 1;
    }
#endif
    // Create listening socket
    socket_t sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd == INVALID_SOCK)
    {
        return 1;
    }

    // Configure server address
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Bind socket to address
    int bindResult =
        bind(sockfd, (const struct sockaddr *)&serverAddr, sizeof(serverAddr));

    if (bindResult == SOCKET_ERROR_CODE)
    {
        close(sockfd);
        return 1;
    }

    // Start listening
    int listenResult = listen(sockfd, SOMAXCONN);

    if (listenResult == SOCKET_ERROR_CODE)
    {
	close(sockfd);
	return 1;
    }

    // Accept a client connection
    while (1)
    {

        struct sockaddr_in clientAddr;
        memset(&clientAddr, 0, sizeof(clientAddr));

        addrlen_t clientAddrSize = sizeof(clientAddr);

        socket_t clientSocket = accept(sockfd, (struct sockaddr *)&clientAddr, &clientAddrSize);

        if (clientSocket == INVALID_SOCK)
        {
            close(sockfd);
            return 1;
        }

        // Receive request
        int received = recv(clientSocket, recvBuffer, DEFAULT_BUF_LEN - 1, 0);

        if (received > 0)
        {
            recvBuffer[received] = '\0';
            printf("%s\n", recvBuffer);
        }
        else if (received == 0)
        {
            close(clientSocket);
            continue;
        }
        else
        {

            close(clientSocket);
            close(sockfd);
            return 1;
        }

        // Send HTTP response
        char *filename = "../public/index.html";
        char *contentType = "text/html";
        if (sscanf(recvBuffer, "%15s %255s", method, path) != 2)
        {
            close(clientSocket);
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
        long fileSize;
        char *htmlBuffer;
        if (htmlFile == NULL)
        {
            htmlFile = fopen("404.html", "rb");
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
                close(clientSocket);
                continue;
            }

            fseek(htmlFile, 0, SEEK_END);
            fileSize = ftell(htmlFile);
            fseek(htmlFile, 0, SEEK_SET);

            htmlBuffer = malloc((size_t)fileSize + 1);
            if (htmlBuffer == NULL)
            {
                fclose(htmlFile);
                close(clientSocket);
                continue;
            }
            size_t bytesCount = fread(htmlBuffer, 1, fileSize, htmlFile);
            if (bytesCount != fileSize)
            {
                free(htmlBuffer);
                fclose(htmlFile);
                close(clientSocket);
                continue;
            }
            htmlBuffer[fileSize] = '\0';
            snprintf(
                sendBuffer,
                sizeof(sendBuffer),
                "HTTP/1.1 404 Not Found\r\n"
                "Connection: close\r\n"
                "Content-Type: text/html\r\n"
                "Content-Length: %ld\r\n"
                "\r\n",
                fileSize);

            send(clientSocket, sendBuffer, strlen(sendBuffer), 0);
            send(clientSocket, htmlBuffer, (int)fileSize, 0);

            free(htmlBuffer);
            fclose(htmlFile);
            close(clientSocket);
            continue;
        }

        fseek(htmlFile, 0, SEEK_END);
        fileSize = ftell(htmlFile);
        if (fileSize == -1)
        {
            printf("ftell failed\n");
            fclose(htmlFile);
            close(clientSocket);
            continue;
        }
        fseek(htmlFile, 0, SEEK_SET);
        htmlBuffer = malloc((size_t)fileSize + 1);
        if (htmlBuffer == NULL)
        {
            printf("Memory Allocation failed!\n");
            close(clientSocket);
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
            close(clientSocket);
            free(htmlBuffer);
            fclose(htmlFile);
            return 1;
        }
        // Get Number of Bytes sent
        int header_bytes_sent = send(clientSocket, sendBuffer, strlen(sendBuffer), 0);
        int body_bytes_sent = send(clientSocket, htmlBuffer, (int)fileSize, 0);

        if (body_bytes_sent == -1 || header_bytes_sent == -1)
        {
            fclose(htmlFile);
            close(clientSocket);
            free(htmlBuffer);
            continue;
        }
        free(htmlBuffer);
        fclose(htmlFile);
        close(clientSocket);
    }


    close(sockfd);
    return 0;
}
