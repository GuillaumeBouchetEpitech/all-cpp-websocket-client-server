
# All C++ WebSocket Client Server

Client-server example

## The Client Side:
* is written in C++ and compiled to WebAssembly
  * which offer the (currently) highest performance possible within a browser
* will connect to the server with a websocket inside the WebAssembly logic
  * the connection is established using a config fetched from the server-side

## The Server Side:
* is written in C++ and compiled to a native binary
* the network (Http and WebSockets) use boost Asio beast
  * rely on boost executor(s), which rely on boost strand
  * the number of thread used can be configured
### http server:
* recursively explore an asset folder
  * open every files (when possible)
  * compress the opened files (when possible)
  * cache in-memory (hash map)
* accept and reply to HTTP request (GET)
  * handle any custom request9s
    * used to reply the "web-socket-config.json"
  * retrieve the requested file from the cache
    * reply the compressed payload (when available and supported)
      * otherwise reply the original payload
    * will set the content-type header (when supported)
  * 404 when the requested file is not found in the in-memory cache
### websocket server:
* accept connection and broadcast new user
* read messages and broadcast to all users
* handle disconnection and broadcast as lost user
* could be used as an example of (efficient?) thread safe session manager
* and example of (efficient?) thread safe session manager is included
  * one `shared mutex` is used
    * multiple read with `shared lock(s)`
    * unique write with `unique lock(s)`


# Dependencies

## Dependency: Emscripten 3.1.26 (client-side build)
```bash
git clone https://github.com/emscripten-core/emsdk.git

cd emsdk

./emsdk install 3.1.26
./emsdk activate --embedded 3.1.26

. ./emsdk_env.sh

em++ --clear-cache
```

## Dependency: zlib, boost (server-side build)

```
libzstd1
libboost1.71-dev
```

# How to Build

## Build Everything (client + server side, also run the server)

```bash
chmod +x ./sh_everything.sh
./sh_everything.sh
# will:
# -> build the client C++ application (wasm)
# -> build the client TypeScript wasm-loader (JavaScript)
# -> build the server C++ application (binary)
# -> copy the client files in the server asset folder
# -> run the server
```

# How to Run

## Step 1 - Run The Server Side

```bash
cd ./server-side
#           ip-address  http-port  ws-port  total-threads
./bin/exec  127.0.0.1   7777       8888     1
```

## Step 2 - Run The Client Side

```bash
# http://{ip-address}:{http-port}/
http://127.0.0.1:7777/
```

# Thanks for watching!
