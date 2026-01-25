
#pragma once

#if defined __EMSCRIPTEN__
#error this file should only be included for a native build
#endif

#include "../AbstractWebSocketServer.hpp"
#include "../callbacks.hpp"

#include "../../common/AbstractTcpListener.hpp"

#include <thread>
#include <vector>

class WebSocketServer : public AbstractWebSocketServer {

public:
  explicit WebSocketServer(
    const std::string& ipAddress,
    uint16_t port,
    uint32_t totalThreads = 1,
    bool useStrands = false);

  WebSocketServer(const WebSocketServer& other) = delete;
  WebSocketServer(WebSocketServer&& other) = delete;
  WebSocketServer& operator=(const WebSocketServer& other) = delete;
  WebSocketServer& operator=(WebSocketServer&& other) = delete;

  ~WebSocketServer();

public:
  void setOnConnectionCallback(const ws_callbacks::OnConnection& onConnectionCallback) override;
  void setOnDisconnectionCallback(const ws_callbacks::OnDisconnection& onDisconnectionCallback) override;
  void setOnMessageCallback(const ws_callbacks::OnMessage& onMessageCallback) override;

public:
  void start() override;
  void stop() override;

private:
  net::io_context _ioc;
  std::shared_ptr<net::strand<net::any_io_executor>> _sharedStrand;
  uint32_t _totalThreads;
  bool _useStrands;
  std::shared_ptr<AbstractTcpListener> _mainTcpListener;
  std::vector<std::thread> _allThreads;

  ws_callbacks::AllCallbacks _allCallbacks;
};
