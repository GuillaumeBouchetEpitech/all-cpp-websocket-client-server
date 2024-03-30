#pragma once

#include <emscripten/emscripten.h>
#include <emscripten/websocket.h>

#include <cstring> // memset, memcpy
#include <functional>

class WebSocketConnection {

public:
  using OnOpenCallback = std::function<void(int, const EmscriptenWebSocketOpenEvent*)>;
  using OnErrorCallback = std::function<void(int, const EmscriptenWebSocketErrorEvent*)>;
  using OnCloseCallback = std::function<void(int, const EmscriptenWebSocketCloseEvent*)>;
  using OnMessageCallback = std::function<void(int, const EmscriptenWebSocketMessageEvent*)>;

private:
  bool _isConnected = false;
  EMSCRIPTEN_WEBSOCKET_T _wsSocket;

private:
  OnOpenCallback _onOpenCallback = nullptr;
  OnErrorCallback _onErrorCallback = nullptr;
  OnCloseCallback _onCloseCallback = nullptr;
  OnMessageCallback _onMessageCallback = nullptr;

public:
  WebSocketConnection() = default;
  WebSocketConnection(const WebSocketConnection& other) = delete;
  WebSocketConnection(WebSocketConnection&& other) = delete;
  WebSocketConnection& operator=(const WebSocketConnection& other) = delete;
  WebSocketConnection& operator=(WebSocketConnection&& other) = delete;
  virtual ~WebSocketConnection();

public:
  WebSocketConnection& setOnOpenCallback(const OnOpenCallback& inOnOpenCallback);
  WebSocketConnection& setOnErrorCallback(const OnErrorCallback& inOnErrorCallback);
  WebSocketConnection& setOnCloseCallback(const OnCloseCallback& inOnCloseCallback);
  WebSocketConnection& setOnMessageCallback(const OnMessageCallback& inOnMessageCallback);

public:
  void connect(const char* url);

  void disconnect();

public:
  bool sendUtf8Text(const char* inText);
  bool sendBinary(void* inData, std::size_t inSize);

private:
  static EM_BOOL _emOnOpen(int eventType, const EmscriptenWebSocketOpenEvent* websocketEvent, void* userData);
  static EM_BOOL _emOnError(int eventType, const EmscriptenWebSocketErrorEvent* websocketEvent, void* userData);
  static EM_BOOL _emOnClose(int eventType, const EmscriptenWebSocketCloseEvent* websocketEvent, void* userData);
  static EM_BOOL _emOnMessage(int eventType, const EmscriptenWebSocketMessageEvent* websocketEvent, void* userData);

private:
  void _onOpen(int eventType, const EmscriptenWebSocketOpenEvent* websocketEvent);
  void _onError(int eventType, const EmscriptenWebSocketErrorEvent* websocketEvent);
  void _onClose(int eventType, const EmscriptenWebSocketCloseEvent* websocketEvent);
  void _onMessage(int eventType, const EmscriptenWebSocketMessageEvent* websocketEvent);
};
