
#pragma once

#include "internal/TcpListener.hpp"

// #include <iostream>
#include <thread>
#include <vector>

class WebSocketServer {
private:
  using Sessions = std::vector<std::shared_ptr<WebSocketSession>>;

  net::io_context _ioc;
  uint32_t _totalThreads;
  std::shared_ptr<TcpListener> _mainTcpListener;
  std::vector<std::thread> _allThreads;

public:
  using OnConnectionCallback = TcpListener::OnConnectionCallback;
  using OnDisconnectionCallback = TcpListener::OnDisconnectionCallback;
  using OnMessageCallback = TcpListener::OnMessageCallback;

public:
  WebSocketServer(
    boost::asio::ip::address inAddress, uint16_t inPort,
    uint32_t inTotalThreads = 1);

  WebSocketServer(const WebSocketServer& other) = delete;
  WebSocketServer(WebSocketServer&& other) = delete;
  WebSocketServer& operator=(const WebSocketServer& other) = delete;
  WebSocketServer& operator=(WebSocketServer&& other) = delete;

  ~WebSocketServer();

public:
  void setOnConnectionCallback(OnConnectionCallback onConnectionCallback);

  void
  setOnDisconnectionCallback(OnDisconnectionCallback onDisconnectionCallback);

  void setOnMessageCallback(OnMessageCallback onMessageCallback);

public:
  void start();
  void stop();
};
