# HTTP Server in C (Linux Version)

A simple HTTP server written from scratch in C using the Linux socket API.

This project was built to understand how web servers work internally by implementing networking, request parsing, routing, and HTTP response generation without relying on external frameworks.

The server listens for TCP connections, parses incoming HTTP requests, serves static files, and returns custom HTTP responses including a custom 404 page.

---

## Features

* TCP server built using Linux sockets
* HTTP/1.1 request handling
* Support for GET requests
* Serves static files

  * HTML
  * CSS
  * JavaScript
* Dynamic routing based on URL paths
* Content-Type detection
* Custom 404 page support
* Sequential client handling
* Manual HTTP response generation
* Automatic local IP address discovery
* LAN access support

---

## Technologies

* C
* Linux Socket API
* TCP/IP
* HTTP/1.1
* GCC
* Neovim

---

## Screenshots

### Server Startup

The server automatically discovers and displays available network interfaces.

This allows the application to be accessed locally and from other devices connected to the same network.

![Server Startup](screenshots/server-startup.png)

---

### HTTP Request Logging

Incoming browser requests are printed directly to the terminal.

This makes it easier to understand how browsers communicate with web servers.

![HTTP Request Logging](screenshots/http-request.png)

---

### Homepage

The default page served by the HTTP server.

![Homepage](screenshots/homepage.png)

---

### Custom 404 Page

When a requested file cannot be found, the server serves a custom 404 page.

![404 Page](screenshots/404-page.png)

---


### Development Workflow

The project was developed on Linux using Neovim and GCC.

![Neovim Workflow](screenshots/nvim-workflow.png)

---

## How It Works

### 1. Create a Socket

The server creates a TCP socket using:

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

### 2. Configure the Server Address

The server listens on port 8080.

```c
serverAddr.sin_family = AF_INET;
serverAddr.sin_port = htons(8080);
serverAddr.sin_addr.s_addr = INADDR_ANY;
```

Using `INADDR_ANY` allows the server to accept connections from all available network interfaces in the current network.

---

### 3. Bind the Socket

```c
bind(sockfd,
     (const struct sockaddr *)&serverAddr,
     sizeof(serverAddr));
```

With bind we associate the socket with a local address and port.

---

### 4. Start Listening

```c
listen(sockfd, SOMAXCONN);
```

The operating system begins accepting incoming TCP connection requests.

---

### 5. Display Available Network Interfaces

The server uses:

```c
getifaddrs()
```

to enumerate network interfaces and print all available IPv4 addresses.

Example:

```text
Server listening on:

http://127.0.0.1:8080
http://192.168.100.226:8080
```

This allows access from:

* The local machine
* Other devices on the same network

---

### 6. Accept Client Connections

The server continuously waits for new clients:

```c
while (1)
{
    accept(...);
}
```

Each accepted connection receives its own client socket.
With each request memory gets allocated and freed

---

### 7. Receive HTTP Requests

Incoming requests are read using:

```c
recv(...)
```

Example request:

```http
GET / HTTP/1.1
Host: 192.168.100.226:8080
User-Agent: Firefox
```

The raw request is printed to the terminal for debugging and learning purposes.

---

### 8. Parse the Request

The request method and path are extracted using:

```c
sscanf(recvBuffer,
       "%15s %255s",
       method,
       path);
```

Example:

```text
Method: GET
Path: /style.css
```

---

### 9. Route Requests

The requested path determines which file should be served.

Examples:

| Request    | File Served |
| ---------- | ----------- |
| /          | index.html  |
| /style.css | style.css   |
| /script.js | script.js   |

---

### 10. Detect Content Type

The server determines the correct MIME type.

Examples:

```text
text/html
text/css
application/javascript
```

This allows browsers to properly interpret files.

---

### 11. Load the Requested File

Files are opened using:

```c
fopen(filename, "rb");
```

The file contents are loaded into memory using:

```c
fread(...)
```

---

### 12. Build the HTTP Response

Response headers are manually created using:

```c
snprintf(...)
```

Example:

```http
HTTP/1.1 200 OK
Connection: close
Content-Type: text/html
Content-Length: 559
```

---

### 13. Send the Response

The response is transmitted in two stages.

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
     fileSize,
     0);
```

---

### 14. Handle Missing Files

If a requested file cannot be found:

```c
fopen(...) == NULL
```

the server serves a custom 404 page.

Example response:

```http
HTTP/1.1 404 Not Found
```

---

## Example Request

```http
GET /style.css HTTP/1.1
Host: 192.168.100.226:8080
```

---

## Example Response

```http
HTTP/1.1 200 OK
Content-Type: text/css
Content-Length: 1372
```

---

## Project Structure

```text
c-http-server/
│
├── server.c
├── index.html
├── style.css
├── 404.html
├── screenshots/
│   ├── homepage.png
│   ├── 404-page.png
│   ├── server-startup.png
│   ├── http-request.png
│   └── nvim-workflow.png
│
├── .gitignore
└── README.md
```

---

## Installation

### Clone the Repository

```bash
git clone https://github.com/oussama-dalhi/c-http-server.git
cd c-http-server
```

### Compile

```bash
gcc -o server server.c
```

### Run

```bash
./server
```

Expected output:

```text
Server listening on:

http://127.0.0.1:8080
http://192.168.100.226:8080
```

---

## Accessing the Server

From the same machine:

```text
http://127.0.0.1:8080
```

From another device on the same network:

```text
http://YOUR_LOCAL_IP:8080
```

Example:

```text
http://192.168.100.226:8080
```

---

## What I Learned

Through this project I gained practical experience with:

* Linux socket programming
* TCP/IP networking
* HTTP request parsing
* HTTP response generation
* MIME types
* Client-server architecture
* Memory allocation
* File I/O
* Network interface enumeration
* Browser-server communication
* Debugging network applications

---

## Future Improvements

Potential enhancements include:

* Multithreading
* Thread pools
* Keep-Alive connections
* Serving images
* Additional MIME types
* Better request parsing
* Directory traversal protection
* Logging system
* Configuration file support
* HTTP POST support

---

## Author

Oussama Dalhi

GitHub: https://github.com/oussama-dalhi
