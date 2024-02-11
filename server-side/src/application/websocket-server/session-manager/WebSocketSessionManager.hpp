
#pragma once

#include "network-wrapper/websocket-server/IWebSocketSession.hpp"

#include <functional>
#include <shared_mutex>
#include <vector>
#include <memory>

class WebSocketSessionManager {

private:
  using SessionPtr = std::shared_ptr<IWebSocketSession>;
  using SessionPtrs = std::vector<SessionPtr>;
  SessionPtrs _allSessions;

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
  void addSession(SessionPtr wsSession);
  void removeSession(SessionPtr wsSession);
  void forEachSession(const std::function<void(SessionPtr)>& callback);
  void forEachSession(const std::function<void(SessionPtr)>& callback) const;
};
