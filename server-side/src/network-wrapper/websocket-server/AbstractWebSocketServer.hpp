
#pragma once

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
  AbstractWebSocketServer&
  operator=(const AbstractWebSocketServer& other) = delete;
  AbstractWebSocketServer& operator=(AbstractWebSocketServer&& other) = delete;

public:
  virtual void setOnConnectionCallback(
    const ws_callbacks::OnConnection& onConnectionCallback) = 0;
  virtual void setOnDisconnectionCallback(
    const ws_callbacks::OnDisconnection& onDisconnectionCallback) = 0;
  virtual void
  setOnMessageCallback(const ws_callbacks::OnMessage& onMessageCallback) = 0;

public:
  virtual void start() = 0;
  virtual void stop() = 0;

public:
  static std::unique_ptr<AbstractWebSocketServer> create(
    const std::string& inIpAddress, uint16_t inPort,
    uint32_t inTotalThreads = 1);
};
