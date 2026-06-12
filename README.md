# HTTP Server in C

A simple HTTP server written from scratch in C using the Winsock API.

The project was created to better understand how web servers work internally by implementing the core networking concepts manually instead of relying on frameworks.

## Features

- TCP server built with Winsock
- HTTP request parsing
- Support for GET requests
- Serves static files
  - HTML
  - CSS
  - JavaScript
- Dynamic file routing
- Content-Type detection
- 404 Not Found handling
- Multiple client requests handled sequentially
- Manual HTTP response generation

---

## Technologies

- C
- Winsock2
- TCP/IP
- HTTP/1.1

---

## How It Works

### 1. Initialize Winsock

The server starts by initializing the Windows Sockets API.

```c
WSAStartup(MAKEWORD(2, 2), &wsaData);
```

This prepares the networking subsystem for socket operations.

---

### 2. Create a TCP Socket

A TCP socket is created using:

```c
socket(AF_INET, SOCK_STREAM, 0);
```

Parameters:

| Value       | Meaning          |
| ----------- | ---------------- |
| AF_INET     | IPv4             |
| SOCK_STREAM | TCP              |
| 0           | Default protocol |

---

### 3. Configure Server Address

The server is configured to listen on:

```text
127.0.0.1:8080
```

```c
serverAddr.sin_family = AF_INET;
serverAddr.sin_port = htons(8080);
InetPton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);
```

---

### 4. Bind the Socket

The socket is bound to the selected IP address and port.

```c
bind(sockfd, ...);
```

This reserves the address for the server.

---

### 5. Start Listening

The server enters listening mode.

```c
listen(sockfd, SOMAXCONN);
```

At this point the operating system begins accepting incoming TCP connection requests.

---

### 6. Accept Client Connections

The server enters an infinite loop:

```c
while (1)
```

Each iteration:

```c
accept(...)
```

creates a new client socket for communication.

---

### 7. Receive HTTP Requests

The browser sends requests such as:

```http
GET /index.html HTTP/1.1
Host: 127.0.0.1:8080
```

The server reads the request using:

```c
recv(...)
```

and stores it inside:

```c
recvBuffer
```

---

### 8. Parse Method and Path

The first request line is parsed using:

```c
sscanf(recvBuffer, "%15s %225s", method, path);
```

Example:

```text
Method: GET
Path: /index.html
```

---

### 9. Route Requests

The requested path determines which file should be served.

Examples:

| Request    | File       |
| ---------- | ---------- |
| /          | index.html |
| /style.css | style.css  |
| /script.js | script.js  |

```c
if(strcmp(path, "/") != 0)
{
    filename = path + 1;
}
```

---

### 10. Detect Content Type

The server selects the correct MIME type.

```c
text/html
text/css
application/javascript
```

Example:

```c
if(strstr(filename, ".css"))
{
    contentType = "text/css";
}
```

This allows browsers to correctly interpret resources.

---

### 11. Load File Contents

Files are opened using:

```c
fopen(filename, "rb");
```

The contents are read into memory using:

```c
fread(...)
```

---

### 12. Generate HTTP Response

The server manually builds an HTTP response header.

Example:

```http
HTTP/1.1 200 OK
Connection: close
Content-Type: text/html
Content-Length: 559
```

using:

```c
snprintf(...)
```

---

### 13. Send Response

The response is transmitted in two steps:

#### Header

```c
send(clientSocket,
     sendBuffer,
     strlen(sendBuffer),
     0);
```

#### Body

```c
send(clientSocket,
     htmlBuffer,
     readBytes,
     0);
```

---

### 14. Handle Missing Files

If a requested file does not exist:

```c
fopen(...) == NULL
```

the server returns:

```http
HTTP/1.1 404 Not Found
```

and closes the connection.

---

## Example Request

```http
GET /style.css HTTP/1.1
Host: 127.0.0.1:8080
```

## Example Response

```http
HTTP/1.1 200 OK
Content-Type: text/css
Content-Length: 1372
```

---

## Project Structure

```text
http-server/
│
├── server.c
├── index.html
├── style.css
├── README.md
└── .gitignore
```

---

## What I Learned

Through this project I gained practical experience with:

- TCP/IP networking
- Socket programming
- HTTP request and response structure
- MIME types
- Client-server communication
- File I/O in C
- Memory management
- Debugging network applications
- Browser-server interactions

---

## Future Improvements

Potential enhancements include:

- Persistent connections (Keep-Alive)
- Serving images
- Multithreading
- HTTP request parsing improvements
- Directory traversal protection

---

## Running the Project

Compile:

```bash
gcc -o server server.c -lws2_32
```

Run:

```bash
./server
```

Open:

```text
http://127.0.0.1:8080
```

in a web browser.

---

## Author

Oussama Dalhi

GitHub: https://github.com/oussama-dalhi
