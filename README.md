
# All C++ WebSocket Client Server

## Table of Contents
- [All C++ WebSocket Client Server](#all-c-websocket-client-server)
  - [Table of Contents](#table-of-contents)
- [Description](#description)
  - [The Client Side:](#the-client-side)
  - [The Server Side:](#the-server-side)
    - [Http Server:](#http-server)
    - [WebSocket Server:](#websocket-server)
  - [ The Network Wrapper:](#-the-network-wrapper)
- [Dependencies](#dependencies)
  - [Client-Side:](#client-side)
  - [Server-Side: zlib, boost](#server-side-zlib-boost)
- [How to Build](#how-to-build)
  - [Build Everything (client + server side, also run the server)](#build-everything-client--server-side-also-run-the-server)
- [How to Run](#how-to-run)
  - [Step 1 - Run The Server](#step-1---run-the-server)
  - [Step 2 - Load The Client](#step-2---load-the-client)
- [Thanks for watching!](#thanks-for-watching)

# Description

C++ Client-Server example

## The Client Side:
* is written in C++ and compiled to WebAssembly
  * which offer the (currently) highest performance possible within a browser
    * `1.7x slower than native speed`
    * `or ~5-6 times faster than JavaScript` (also devoid of GC slowdown)
* will connect to the server with a websocket inside the WebAssembly logic
  * the connection is established using a config fetched from the server-side (easily configured if needed)
* `a native version` of the client is also available
  * `mostly for` faster development and `debug purpose`:
    * tools like gdb and valgrind become available
* the network part rely on the [Network Wrapper](#network-wrapper-anchor)

```mermaid

  flowchart TB

    subgraph client-side [Client Side]

      browser[Browser]

      subgraph client-js-app [Web Js App]

        direction TB

        file_index_html[index.html]
        file_bundle_js[bundle.js]

        file_index_html -..-> file_bundle_js
      end

      subgraph client-wasm-app [Web Wasm App]

        direction TB

        file_index_js[index.js]
        file_index_wasm[index.wasm]
        file_index_data[index.data]

        file_index_js -..-> file_index_wasm
        file_index_js -..-> file_index_data

      end

      subgraph real-time-client-app ["Real Time Client App (Example)"]

        direction LR

        ws_client[WebSocket<br>Client]
        client_game_logic[Game Logic]
        client_physic_logic[Physic]
        client_graphic_logic[Graphic]

        client_game_logic <---> ws_client
        client_game_logic <-.-> client_physic_logic
        client_game_logic <-.-> client_graphic_logic
      end

      browser --- client-js-app
      client-js-app --> client-wasm-app
      client-wasm-app --- real-time-client-app

      client_http_get((HTTP GET))
      client_websocket((WebSocket))

      client-js-app --- client_http_get
      client-wasm-app --- client_http_get
      real-time-client-app --- client_websocket

    end



```

## The Server Side:
* is written in C++ and compiled to a native binary
* the network rely on the [Network Wrapper](#network-wrapper-anchor)
* the number of thread(s) used can be configured by command line arguments

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


```mermaid

  flowchart TD

    subgraph server-side [Server Side]

      direction LR

      server_http_get((HTTP GET))

      subgraph server-files [File Server]

        http_server[HTTP Server]

        direction LR

        server_file_cache[File Cache]
        server_file_disk[Disk]

        http_server <---> server_file_cache
        server_file_cache <-.-> server_file_disk

      end

      server_http_get --- server-files

    end

```

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



```mermaid

  flowchart TD

    subgraph server-side [Server Side]

      direction LR

      server_websocket((WebSocket))

      subgraph real-time-server-app ["Real Time Server App (Example)"]

        direction LR

        ws_server[WebSocket<br>Server]
        server_game_logic[Game Logic]
        server_physic_logic[Physic]

        ws_server <---> server_game_logic
        server_game_logic <-.-> server_physic_logic

      end

      server_websocket --- real-time-server-app

    end




```

## <a name="network-wrapper-anchor"></a> The Network Wrapper:
* is written in C++ and compiled to a native archive or a wasm bytecode
  * the native part contains
    * an abstracted http server
      * host, port, total-threads (boost::strand if 2 or more threads)
    * an abstracted websocket server
      * host, port, total-threads (boost::strand if 2 or more threads)
    * an abstracted websocket client
      * rely on boost beast websocket
      * use a worker thread
  * the wasm part contains
    * an abstracted websocket client
      * rely on emscripten websocket
* the native parts rely on boost Asio beast
  * rely on boost executor(s)
    * which in turn, rely on boost strand
      * [which is a more efficient way to use multi-threading](https://www.crazygaze.com/blog/2016/03/17/how-strands-work-and-why-you-should-use-them/)
* benefit(s) of the wrapper
  * easily reusable
  * make boost (very) slow compilation happens only once (thanks to [pimpl](https://en.cppreference.com/w/cpp/language/pimpl.html))
  * allow the client-side to work in native and wasm with minimal code differences


* all of the network code is abstracted and located in the network-wrapper
  * native wrapper: native archive (static library)
  * wasm wrapper: wasm bytecode (static library)



```mermaid

mindmap
  root(("network<br>wrapper"))
    http_server)"http server"(
    ws_server)"websocket<br>server"(
    ws_client)"websocket<br>native/wasm<br>client"(

```



# Dependencies

## Client-Side:

- Emscripten 3.1.44
  - This will be locally installed (cloned and setup) if not detected
  - the 3.1.44 version will be ensured, even if already present
- NodeJs

## Server-Side: zlib, boost

- g++ (build-essential)
- libzstd1
- libboost1.71-dev

# How to Build

## Build Everything (client + server side, also run the server)

```bash
sh ./scripts/sh_build_with_cmake.sh
# will:
# -> build the client C++ application (wasm)
# -> build the client TypeScript wasm-loader (JavaScript)
# -> build the server C++ application (binary)
# -> copy the client files in the server asset folder
# -> run the server
#    -> http-port: 777
#    -> ws-port: 8888
#    -> threads all at 1
```

# How to Run

## Step 1 - Run The Server

```bash
# the server need to be started from the "./server-side" folder
# -> mainly since this is where the "files/" folder is located
cd ./samples/basic-client-server/server-side

#             ip-address  http-port  ws-port  http-threads  ws-threads
./bin/server  127.0.0.1   7777       8888     1             1
```

## Step 2 - Load The Client

```bash
# http://{ip-address}:{http-port}/
http://127.0.0.1:7777/

# to see it in action -> open more tabs with the same url
# -> will log broadcasts of when a new clients join/leave
```

# Thanks for watching!
