
#pragma once

#include "../AbstractWebSocketSessionManager.hpp"

#include <shared_mutex>

class ThreadSafeWebSocketSessionManager : public AbstractWebSocketSessionManager {

private:
  mutable std::shared_mutex _mutex;

public:
  ThreadSafeWebSocketSessionManager() = default;
  virtual ~ThreadSafeWebSocketSessionManager() = default;

public:
  void addSession(AbstractWebSocketSessionManager::SessionPtr wsSession);
  void removeSession(AbstractWebSocketSessionManager::SessionPtr wsSession);
  void forEachSession(const std::function<void(AbstractWebSocketSessionManager::SessionPtr)>& callback);
  void forEachSession(const std::function<void(AbstractWebSocketSessionManager::SessionPtr)>& callback) const;
};
