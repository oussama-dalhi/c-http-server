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
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
typedef int socket_t;
typedef socklen_t addrlen_t;
#define SOCKET_ERROR_CODE (-1)
#define INVALID_SOCK (-1)
#define close_socket close
#endif

#define DEFAULT_BUF_LEN 4096

char recvBuffer[DEFAULT_BUF_LEN];

char method[16];
char path[256];

static int send_all(socket_t s, const char *data, size_t len)
{
    size_t sent = 0;
    while (sent < len)
    {
        int n = send(s, data + sent, (int)(len - sent), 0);
        if (n <= 0)
            return -1;
        sent += (size_t)n;
    }
    return 0;
}

static const char *get_content_type(const char *filename)
{
    const char *ext = strrchr(filename, '.');
    if (ext == NULL)
        return "application/octet-stream";
    if (strcmp(ext, ".html") == 0)
        return "text/html";
    if (strcmp(ext, ".css") == 0)
        return "text/css";
    if (strcmp(ext, ".js") == 0)
        return "application/javascript";
    if (strcmp(ext, ".png") == 0)
        return "image/png";
    if (strcmp(ext, ".jpg") == 0)
        return "image/jpeg";
    if (strcmp(ext, ".svg") == 0)
        return "image/svg+xml";
    return "application/octet-stream";
}

static int send_file(socket_t client, const char *filename,
                     const char *status, const char *contentType)
{
    FILE *f = fopen(filename, "rb");
    if (f == NULL)
        return -1;

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (size < 0)
    {
        fclose(f);
        return -1;
    }

    char *body = malloc((size_t)size + 1);
    if (body == NULL)
    {
        fclose(f);
        return -1;
    }

    if (fread(body, 1, (size_t)size, f) != (size_t)size)
    {
        free(body);
        fclose(f);
        return -1;
    }
    fclose(f);

    char header[256];
    int n = snprintf(header, sizeof(header),
                     "HTTP/1.1 %s\r\n"
                     "Connection: close\r\n"
                     "Content-Type: %s\r\n"
                     "Content-Length: %ld\r\n"
                     "\r\n",
                     status, contentType, size);

    send_all(client, header, (size_t)n);
    send_all(client, body, (size_t)size);
    free(body);
    return 0;
}

int main(int argc, char *argv[])
{
#ifdef _WIN32
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0)
    {
        printf("WSAStartup failed: %d\n", result);
        return 1;
    }
#else
    signal(SIGPIPE, SIG_IGN);
#endif

    const char *root = argc > 1 ? argv[1] : ".";
    // Create listening socket
    socket_t sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == INVALID_SOCK)
    {
        printf("socket failed\n");
        return 1;
    }

#ifndef _WIN32
    int yes = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
#endif

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Bind socket to address
    if (bind(sockfd, (const struct sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR_CODE)
    {
        printf("bind failed\n");
        close_socket(sockfd);
        return 1;
    }

    // Start listening
    if (listen(sockfd, SOMAXCONN) == SOCKET_ERROR_CODE)
    {
        printf("listen failed\n");
        close_socket(sockfd);
        return 1;
    }

    printf("Server listening on port 8080\n");
    printf("Local:   http://localhost:8080\n");
    printf("Network: http://YOUR_LOCAL_IP:8080\n");

    while (1)
    {
        struct sockaddr_in clientAddr;
        memset(&clientAddr, 0, sizeof(clientAddr));
        addrlen_t clientAddrSize = sizeof(clientAddr);

        socket_t clientSocket = accept(sockfd, (struct sockaddr *)&clientAddr, &clientAddrSize);
        if (clientSocket == INVALID_SOCK)
        {
            continue;
        }

        // Receive request
        int received = recv(clientSocket, recvBuffer, DEFAULT_BUF_LEN - 1, 0);
        if (received <= 0)
        {
            close_socket(clientSocket);
            continue;
        }
        recvBuffer[received] = '\0';
        printf("%s\n", recvBuffer);

        if (sscanf(recvBuffer, "%15s %255s", method, path) != 2)
        {
            close_socket(clientSocket);
            continue;
        }

        // Only GET
        if (strcmp(method, "GET") != 0)
        {
            const char *msg =
                "HTTP/1.1 405 Method Not Allowed\r\n"
                "Allow: GET\r\n"
                "Connection: close\r\n"
                "Content-Length: 0\r\n"
                "\r\n";
            send_all(clientSocket, msg, strlen(msg));
            close_socket(clientSocket);
            continue;
        }

        char *q = strchr(path, '?');
        if (q != NULL)
            *q = '\0';

        char filename[1024];
        if (strcmp(path, "/") == 0)
            snprintf(filename, sizeof(filename), "%s/index.html", root);
        else
            snprintf(filename, sizeof(filename), "%s%s", root, path);

        int safe = (path[0] == '/' && strstr(path, "..") == NULL);

        if (!safe || send_file(clientSocket, filename, "200 OK",
                               get_content_type(filename)) != 0)
        {
            if (send_file(clientSocket, "public/404.html", "404 Not Found", "text/html") != 0)
            {
                const char *msg =
                    "HTTP/1.1 404 Not Found\r\n"
                    "Connection: close\r\n"
                    "Content-Length: 0\r\n"
                    "\r\n";
                send_all(clientSocket, msg, strlen(msg));
            }
        }

        close_socket(clientSocket);
    }
}
