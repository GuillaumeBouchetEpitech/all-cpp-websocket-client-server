
#pragma once

#include "network-wrapper/websocket-server/IWebSocketSession.hpp"

#include <functional>
#include <vector>
// #include <mutex>
#include <shared_mutex>
// #include <thread>

class WebSocketSessionManager {

private:
  using Sessions = std::vector<std::shared_ptr<IWebSocketSession>>;
  Sessions _allSessions;

  mutable std::shared_mutex _mutex;

public:
  WebSocketSessionManager();
  WebSocketSessionManager(const WebSocketSessionManager& other) = delete;
  WebSocketSessionManager(WebSocketSessionManager&& other) = delete;
  WebSocketSessionManager&
  operator=(const WebSocketSessionManager& other) = delete;
  WebSocketSessionManager& operator=(WebSocketSessionManager&& other) = delete;
  virtual ~WebSocketSessionManager() = default;

public:
  void addSession(std::shared_ptr<IWebSocketSession> inWsSession);
  void removeSession(std::shared_ptr<IWebSocketSession> inWsSession);
  void forEachSession(
    const std::function<void(std::shared_ptr<IWebSocketSession>)>& inCallback);
};
