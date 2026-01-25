
#include "ThreadUnsafeWebSocketSessionManager.hpp"

#include "network-wrapper/utilities/stdVectorNoReallocErase.hpp"

#include <stdexcept>

void
ThreadUnsafeWebSocketSessionManager::addSession(SessionPtr wsSession) {

  _allSessions.push_back(wsSession);
}

void
ThreadUnsafeWebSocketSessionManager::removeSession(SessionPtr wsSession) {

  const bool wasRemoved = stdVectorNoReallocEraseByValue(_allSessions, wsSession);
  if (!wasRemoved) {
    throw std::runtime_error("websocket session to remove was not found");
  }
}

void
ThreadUnsafeWebSocketSessionManager::forEachSession(const std::function<void(SessionPtr)>& callback) {

  for (std::size_t index = 0; index < _allSessions.size(); ++index) {
    callback(_allSessions[index]);
  }
}

void
ThreadUnsafeWebSocketSessionManager::forEachSession(const std::function<void(SessionPtr)>& callback) const {

  for (std::size_t index = 0; index < _allSessions.size(); ++index) {
    callback(_allSessions[index]);
  }
}
