
#pragma once

#if defined __EMSCRIPTEN__
#error this file should only be included for a native build
#endif

#include "callbacks.hpp"

// #include "internal/TcpListener.hpp"

// #include <iostream>
#include <thread>
#include <vector>

class AbstractWebSocketServer {
public:
  AbstractWebSocketServer() = default;
  virtual ~AbstractWebSocketServer() = default;

  AbstractWebSocketServer(const AbstractWebSocketServer& other) = delete;
  AbstractWebSocketServer(AbstractWebSocketServer&& other) = delete;
  AbstractWebSocketServer& operator=(const AbstractWebSocketServer& other) = delete;
  AbstractWebSocketServer& operator=(AbstractWebSocketServer&& other) = delete;

public:
  virtual void setOnConnectionCallback(const ws_callbacks::OnConnection& onConnectionCallback) = 0;
  virtual void setOnDisconnectionCallback(const ws_callbacks::OnDisconnection& onDisconnectionCallback) = 0;
  virtual void setOnMessageCallback(const ws_callbacks::OnMessage& onMessageCallback) = 0;

public:
  virtual void start() = 0;
  virtual void stop() = 0;

public:
  [[nodiscard]] static std::shared_ptr<AbstractWebSocketServer>
  create(const std::string& ipAddress, uint16_t port, uint32_t totalThreads = 1, bool useStrands = false);
};
