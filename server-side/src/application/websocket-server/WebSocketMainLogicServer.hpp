
#pragma once

#include "network-wrapper/websocket-server/AbstractWebSocketServer.hpp"

#include "session-manager/WebSocketSessionManager.hpp"

#include <memory>
#include <vector>

class WebSocketMainLogicServer {
private:
  std::unique_ptr<AbstractWebSocketServer> _webSocketServer;
  WebSocketSessionManager _sessionManager;

  int32_t _lastPlayerId = 1;

  struct PlayerData {
    int32_t id = 0;
  };

  std::vector<std::shared_ptr<PlayerData>> _allPlayersData;

public:
  WebSocketMainLogicServer(
    const std::string& ipAddress, uint16_t port, uint32_t totalThreads);

  ~WebSocketMainLogicServer();

public:
  void start();
  void stop();
};
