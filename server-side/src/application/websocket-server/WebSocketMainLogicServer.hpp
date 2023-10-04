
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
    const std::string& inIpAddress, uint16_t inPort, uint32_t inTotalThreads);

  ~WebSocketMainLogicServer();

public:
  void start();
  void stop();
};
