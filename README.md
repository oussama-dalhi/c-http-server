# C HTTP Server

A simple HTTP server written in C using the Windows Winsock API.

## Features

- TCP socket creation
- IPv4 support
- HTTP request handling
- HTTP response generation
- Serves a basic "Hello, World!" page

## Technologies

- C
- Winsock2
- GCC (MinGW)

## How It Works

1. Initialize Winsock
2. Create a TCP socket
3. Bind to `127.0.0.1:8080`
4. Listen for incoming connections
5. Accept a client connection
6. Receive the HTTP request
7. Send an HTTP response
8. Close the connection

## Example Response

HTTP/1.1 200 OK

Hello, World!

## Future Improvements

- Handle multiple clients
- Serve HTML files
- Parse HTTP headers
- Route requests
- Support GET and POST requests
