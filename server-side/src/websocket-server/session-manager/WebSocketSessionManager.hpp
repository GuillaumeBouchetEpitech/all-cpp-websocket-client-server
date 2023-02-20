
#pragma once

#include <functional>
#include <vector>
// #include <mutex>
#include <shared_mutex>
// #include <thread>

class WebSocketSession;

class WebSocketSessionManager {

private:
  using Session = std::vector<std::shared_ptr<WebSocketSession>>;
  Session _allSessions;

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
  void addSession(std::shared_ptr<WebSocketSession> inWsSession);
  void removeSession(std::shared_ptr<WebSocketSession> inWsSession);
  void forEachSession(
    const std::function<void(std::shared_ptr<WebSocketSession>)>& inCallback);
};
