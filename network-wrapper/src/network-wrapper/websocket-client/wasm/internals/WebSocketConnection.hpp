#pragma once

#if not defined __EMSCRIPTEN__
#error this file should only be included for a wasm build
#endif

#include "../../AbstractWebSocketConnection.hpp"

#include <emscripten/emscripten.h>
#include <emscripten/websocket.h>

#include <cstring> // memset, memcpy
// #include <functional>

class WebSocketConnection : public AbstractWebSocketConnection {

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
  AbstractWebSocketConnection& setOnOpenCallback(const OnOpenCallback& inOnOpenCallback) override;
  AbstractWebSocketConnection& setOnErrorCallback(const OnErrorCallback& inOnErrorCallback) override;
  AbstractWebSocketConnection& setOnCloseCallback(const OnCloseCallback& inOnCloseCallback) override;
  AbstractWebSocketConnection& setOnMessageCallback(const OnMessageCallback& inOnMessageCallback) override;

public:
  void connect(std::string_view inHost, std::string_view inPort) override;
  void disconnect() override;

public:
  bool sendUtf8Text(const char* inText) override;
  bool sendBinary(const void* inData, std::size_t inSize) override;

public:
  [[nodiscard]] bool isConnected() const override;

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
