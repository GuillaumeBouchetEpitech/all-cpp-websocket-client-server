
# All C++ WebSocket Client Server

## Table of Contents
- [All C++ WebSocket Client Server](#all-c-websocket-client-server)
  - [Table of Contents](#table-of-contents)
- [Description](#description)
  - [The Client Side:](#the-client-side)
  - [The Server Side:](#the-server-side)
    - [Http Server:](#http-server)
    - [WebSocket Server:](#websocket-server)
- [Dependencies](#dependencies)
  - [Client-Side: Emscripten 3.1.44, bun](#client-side-emscripten-3144-bun)
  - [Server-Side: zlib, boost](#server-side-zlib-boost)
- [How to Build](#how-to-build)
  - [Build Everything (client + server side, also run the server)](#build-everything-client--server-side-also-run-the-server)
- [How to Run](#how-to-run)
  - [Step 1 - Run The Server](#step-1---run-the-server)
  - [Step 2 - Load The Client](#step-2---load-the-client)
- [Thanks for watching!](#thanks-for-watching)

# Description

Client-server example

## The Client Side:
* is written in C++ and compiled to WebAssembly
  * which offer the (currently) highest performance possible within a browser
    * 1.7x slower than native speed
    * or ~5-6 times faster than JavaScript (also devoid of GC slowdown)
* will connect to the server with a websocket inside the WebAssembly logic
  * the connection is established using a config fetched from the server-side (easily configured if needed)

## The Server Side:
* is written in C++ and compiled to a native binary
* the network (Http and WebSockets) use boost Asio beast
  * rely on boost executor(s)
    * which in turn, rely on boost strand
      * [which is a more efficient way to use multi-threading](https://www.crazygaze.com/blog/2016/03/17/how-strands-work-and-why-you-should-use-them/)
  * the number of thread(s) used can be configured

### Http Server:
* recursively explore an asset folder
  * open every files (when possible)
  * compress the opened files (when possible)
  * cache in-memory (hash map)
    * compressed files must be smaller to be kept in the cache
* accept and reply to HTTP request (GET)
  * handle any custom requests
    * used to reply the "web-socket-config.json"
  * retrieve the requested file from the cache
    * reply the requested file
      * if available and supported, will reply the compressed payload
    * will set the appropriate content-type header (when supported)
  * 404 status if the requested file is missing from the in-memory cache

### WebSocket Server:
* event(s)
  * accept connection(s)
    * register the new user
    * broadcast "new user" to all users
  * new message(s)
    * read messages
    * broadcast "new message" to all users
  * disconnection
    * remove the lost user
    * broadcast "disconnected user" to all users
* a thread safe session manager is included
  * a `shared mutex` is used
    * multiple read will rely on `shared lock(s)`
    * unique write will rely on `unique lock(s)`

# Dependencies

## Client-Side: Emscripten 3.1.44, bun
```bash
git clone https://github.com/emscripten-core/emsdk.git

cd emsdk

./emsdk install 3.1.44
./emsdk activate --embedded 3.1.44

. ./emsdk_env.sh

em++ --clear-cache
```

```bash
npm install -g bun
```

## Server-Side: zlib, boost

```bash
libzstd1
```

```bash
libboost1.71-dev
```

# How to Build

## Build Everything (client + server side, also run the server)

```bash
chmod +x ./sh_build_everything.sh
./sh_build_everything.sh
# will:
# -> build the client C++ application (wasm)
# -> build the client TypeScript wasm-loader (JavaScript)
# -> build the server C++ application (binary)
# -> copy the client files in the server asset folder
# -> run the server
```

# How to Run

## Step 1 - Run The Server

```bash
cd ./server-side
#           ip-address  http-port  ws-port  total-threads
./bin/exec  127.0.0.1   7777       8888     1
```

## Step 2 - Load The Client

```bash
# http://{ip-address}:{http-port}/
http://127.0.0.1:7777/
```

# Thanks for watching!
