
#pragma once

#include "network-wrapper/websocket-server/AbstractWebSocketServer.hpp"

#include "session-manager/WebSocketSessionManager.hpp"

#include <memory>
#include <vector>

class WebSocketMainLogicServer {
private:
  std::shared_ptr<AbstractWebSocketServer> _webSocketServer;
  WebSocketSessionManager _sessionManager;

  int32_t _lastPlayerId = 1;

  struct PlayerData {
    int32_t id = 0;
  };

  std::vector<std::shared_ptr<PlayerData>> _allPlayersData;

public:
  WebSocketMainLogicServer(const std::string& ipAddress, uint16_t port, uint32_t totalThreads, bool useStrands);

  ~WebSocketMainLogicServer();

public:
  void start();
  void stop();

private:
  void _onNewClient(std::shared_ptr<IWebSocketSession> newWsSession);
  void _onNewMessage(std::shared_ptr<IWebSocketSession> wsSession, const char* dataPtr, std::size_t dataLength);
  void _onLostClient(std::shared_ptr<IWebSocketSession> disconnectedWsSession);

};
