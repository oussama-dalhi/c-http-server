# C HTTP Server

A small static file server written from scratch in C. It uses plain sockets (POSIX on Linux/macOS and Winsock on Windows), with no external libraries.

I built it to understand how a web server works underneath: TCP connections, parsing HTTP requests, reading files, and building HTTP responses by hand.

## Preview

![Homepage](assets/homepage.png)

![404 page](assets/404-page.png)

## Features

- Runs on Linux, macOS, and Windows from a single `src/server.c`.
- Serves static files from a configurable document root.
- Supports HTML, CSS, JavaScript, PNG, JPG, and SVG files.
- Supports custom document roots through command-line arguments.
- Defaults to the current working directory when no root is specified.
- Serves `index.html` at `/` when it exists.
- Returns 404 when a requested file cannot be found.
- Custom 404 page (`public/404.html`).
- Sets `Content-Type` and `Content-Length` response headers.
- Basic path traversal protection (rejects paths containing `..`).
- Rejects methods other than GET with `405 Method Not Allowed`.
- Can be accessed by other devices on the same local network.

## Build

### Linux / macOS

```bash
gcc -Wall -Wextra -O2 -o server src/server.c
```

### Windows (MinGW)

```bash
gcc -Wall -Wextra -O2 -o server.exe src/server.c -lws2_32
```

### Using Make

If GNU Make is installed and your project has a configured `Makefile`, build with:

```bash
make
```

On Windows, you can use the MSYS2 UCRT64 terminal:

```bash
cd /c/Users/hp/Programming/c/http-server
make
```

## Usage

The server accepts an optional document root as its first command-line argument.

### 1. Use the default directory

```bash
./server
```

When no argument is provided, the server uses the current working directory (`.`).

For example, if you launch the executable while your terminal is in `C:/Users/hp/Pictures`, it serves files from that directory.

**Important:** The current working directory is where you launch the server, not necessarily where the executable is located.

### 2. Specify a document root

```bash
./server public
```

This serves files from the `public/` directory.

You can also specify an absolute path:

```bash
./server "/c/Users/hp/Pictures"
```

Or:

```bash
./server "D:/websites/portfolio"
```

### 3. Access the server

Open your browser:

[http://localhost:8080](http://localhost:8080)

Press `Ctrl + C` to stop the server.

The server listens on port `8080`.

### 4. Serve individual images

You don't need an `index.html` to serve individual files.

For example, with this directory:

```text
assets/
├── car.jpg
├── logo.png
└── engine.svg
```

Run:

```bash
./server assets
```

Then access individual files:

```text
http://localhost:8080/car.jpg
http://localhost:8080/logo.png
http://localhost:8080/engine.svg
```

When visiting `/`, the server looks for `index.html` in the document root. If it doesn't exist, the request returns 404.

Directory listing is not implemented yet.

## Access from other devices

The server binds to `INADDR_ANY`, allowing connections through available IPv4 network interfaces.

To access it from another device on the same Wi-Fi network:

1. Find your computer's local IP address:

   ```bash
   ipconfig
   ```

2. Find the IPv4 address under your Wi-Fi adapter.

3. Open the following URL on your phone or another device:

   ```text
   http://YOUR_LOCAL_IP:8080
   ```

For example:

```text
http://192.168.31.149:8080
```

Make sure both devices are connected to the same Wi-Fi network and Windows Firewall allows incoming connections on port `8080`.

## Try it with curl

```bash
curl -i localhost:8080/                  # 200 OK if index.html exists
curl -i localhost:8080/style.css         # 200 OK, text/css
curl -i localhost:8080/nope              # 404 Not Found
curl -i --path-as-is "localhost:8080/../src/server.c"  # 404, traversal blocked
curl -i -X POST localhost:8080/          # 405 Method Not Allowed
```

## Project structure

```text
c-http-server/
├── src/
│   └── server.c
├── public/
│   ├── index.html
│   ├── style.css
│   ├── 404.html
│   └── 404.css
├── assets/
│   ├── homepage.png
│   └── 404-page.png
├── Makefile
├── README.md
└── .gitignore
```

## How it works

1. `socket()` creates a TCP listening socket.
2. `bind()` associates it with port `8080`.
3. `listen()` starts listening for incoming connections.
4. `accept()` waits for a client connection.
5. `recv()` reads the HTTP request.
6. `sscanf()` extracts the HTTP method and requested path.
7. The requested path is mapped to a file inside the configured document root.
8. The file is read in binary mode and sent with manually constructed HTTP headers using `send()`.
9. The connection is closed and the server waits for the next client.

## Limitations

- Handles one client at a time.
- Only supports GET requests.
- No directory listing.
- No HTTPS support.
- No support for persistent connections.
- Basic path traversal protection; not intended for production use.
- No authentication or access control.

## Author

Oussama Dalhi · [GitHub](https://github.com/oussama-dalhi)