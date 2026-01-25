
#include "ThreadSafeWebSocketSessionManager.hpp"

#include "network-wrapper/utilities/stdVectorNoReallocErase.hpp"

#include <mutex> // std::unique_lock, std::shared_lock
#include <stdexcept>

void
ThreadSafeWebSocketSessionManager::addSession(SessionPtr wsSession) {

  // only one write at a time
  std::unique_lock uniqueLock(_mutex);

  _allSessions.push_back(wsSession);
}

void
ThreadSafeWebSocketSessionManager::removeSession(SessionPtr wsSession) {

  // only one write at a time
  std::unique_lock uniqueLock(_mutex);

  const bool wasRemoved = stdVectorNoReallocEraseByValue(_allSessions, wsSession);
  if (!wasRemoved) {
    throw std::runtime_error("websocket session to remove was not found");
  }
}

void
ThreadSafeWebSocketSessionManager::forEachSession(const std::function<void(SessionPtr)>& callback) {

  // allow multiple read at the same time
  std::shared_lock sharedLock(_mutex);

  for (std::size_t index = 0; index < _allSessions.size(); ++index) {
    callback(_allSessions[index]);
  }
}

void
ThreadSafeWebSocketSessionManager::forEachSession(const std::function<void(SessionPtr)>& callback) const {

  // allow multiple read at the same time
  std::shared_lock sharedLock(_mutex);

  for (std::size_t index = 0; index < _allSessions.size(); ++index) {
    callback(_allSessions[index]);
  }
}
