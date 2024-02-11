
#pragma once

#include "../AbstractWebSocketServer.hpp"

#include "TcpListener.hpp"

// #include <iostream>
#include <thread>
#include <vector>

class WebSocketServer : public AbstractWebSocketServer {

public:
  explicit WebSocketServer(
    const std::string& ipAddress, uint16_t port,
    uint32_t totalThreads = 1);

  WebSocketServer(const WebSocketServer& other) = delete;
  WebSocketServer(WebSocketServer&& other) = delete;
  WebSocketServer& operator=(const WebSocketServer& other) = delete;
  WebSocketServer& operator=(WebSocketServer&& other) = delete;

  ~WebSocketServer();

public:
  void setOnConnectionCallback(
    const ws_callbacks::OnConnection& onConnectionCallback) override;
  void setOnDisconnectionCallback(
    const ws_callbacks::OnDisconnection& onDisconnectionCallback) override;
  void setOnMessageCallback(
    const ws_callbacks::OnMessage& onMessageCallback) override;

public:
  void start() override;
  void stop() override;

private:
  // using Sessions = std::vector<std::shared_ptr<AbstractWebSocketSession>>;

  net::io_context _ioc;
  uint32_t _totalThreads;
  std::shared_ptr<TcpListener> _mainTcpListener;
  std::vector<std::thread> _allThreads;

  // Sessions allSessions;
  // allSessions.reserve(1024);
};
