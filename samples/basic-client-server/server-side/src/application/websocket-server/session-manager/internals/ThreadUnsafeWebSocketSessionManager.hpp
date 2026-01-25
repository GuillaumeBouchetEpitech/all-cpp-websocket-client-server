
#pragma once

#include "../AbstractWebSocketSessionManager.hpp"

class ThreadUnsafeWebSocketSessionManager : public AbstractWebSocketSessionManager {

public:
  ThreadUnsafeWebSocketSessionManager() = default;
  virtual ~ThreadUnsafeWebSocketSessionManager() = default;

public:
  void addSession(AbstractWebSocketSessionManager::SessionPtr wsSession);
  void removeSession(AbstractWebSocketSessionManager::SessionPtr wsSession);
  void forEachSession(const std::function<void(AbstractWebSocketSessionManager::SessionPtr)>& callback);
  void forEachSession(const std::function<void(AbstractWebSocketSessionManager::SessionPtr)>& callback) const;
};
