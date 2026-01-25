
#pragma once

#include "network-wrapper/websocket-server/IWebSocketSession.hpp"

#include <functional>
#include <memory>
#include <vector>

class AbstractWebSocketSessionManager {

public:
  using SessionPtr = std::shared_ptr<IWebSocketSession>;
  using SessionPtrs = std::vector<SessionPtr>;

protected:
  SessionPtrs _allSessions;

public:
  AbstractWebSocketSessionManager();
  AbstractWebSocketSessionManager(const AbstractWebSocketSessionManager& other) = delete;
  AbstractWebSocketSessionManager(AbstractWebSocketSessionManager&& other) = delete;
  AbstractWebSocketSessionManager& operator=(const AbstractWebSocketSessionManager& other) = delete;
  AbstractWebSocketSessionManager& operator=(AbstractWebSocketSessionManager&& other) = delete;
  virtual ~AbstractWebSocketSessionManager() = default;

public:
  [[nodiscard]]
  static std::shared_ptr<AbstractWebSocketSessionManager> createThreadSafe();

  [[nodiscard]]
  static std::shared_ptr<AbstractWebSocketSessionManager> createThreadUnsafe();

public:
  virtual void addSession(SessionPtr wsSession) = 0;
  virtual void removeSession(SessionPtr wsSession) = 0;
  virtual void forEachSession(const std::function<void(SessionPtr)>& callback) = 0;
  virtual void forEachSession(const std::function<void(SessionPtr)>& callback) const = 0;
};
