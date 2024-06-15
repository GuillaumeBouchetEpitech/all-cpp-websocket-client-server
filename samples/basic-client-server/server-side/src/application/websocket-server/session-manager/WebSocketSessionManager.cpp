
#include "WebSocketSessionManager.hpp"

#include "network-wrapper/utilities/stdVectorNoReallocErase.hpp"

#include <mutex>

WebSocketSessionManager::WebSocketSessionManager(bool isLocking)
  : _isLocking(isLocking)
{
  _allSessions.reserve(1024);
}

void
WebSocketSessionManager::addSession(SessionPtr wsSession) {

  if (_isLocking) {

    // only one write at a time
    std::unique_lock uniqueLock(_mutex);

    _allSessions.push_back(wsSession);

  } else {

    _allSessions.push_back(wsSession);

  }
}

void
WebSocketSessionManager::removeSession(SessionPtr wsSession) {

  if (_isLocking) {

    // only one write at a time
    std::unique_lock uniqueLock(_mutex);

    const bool wasRemoved = stdVectorNoReallocEraseByValue(_allSessions, wsSession);
    if (!wasRemoved) {
      throw std::runtime_error("websocket session to remove was not found");
    }

  } else {

    const bool wasRemoved = stdVectorNoReallocEraseByValue(_allSessions, wsSession);
    if (!wasRemoved) {
      throw std::runtime_error("websocket session to remove was not found");
    }

  }
}

void
WebSocketSessionManager::forEachSession(const std::function<void(SessionPtr)>& callback) {

  if (_isLocking) {

    // allow multiple read at the same time
    std::shared_lock sharedLock(_mutex);

    for (std::size_t index = 0; index < _allSessions.size(); ++index) {
      callback(_allSessions[index]);
    }

  } else {

    for (std::size_t index = 0; index < _allSessions.size(); ++index) {
      callback(_allSessions[index]);
    }

  }
}

void
WebSocketSessionManager::forEachSession(const std::function<void(SessionPtr)>& callback) const {

  if (_isLocking) {

    // allow multiple read at the same time
    std::shared_lock sharedLock(_mutex);

    for (std::size_t index = 0; index < _allSessions.size(); ++index) {
      callback(_allSessions[index]);
    }

  } else {

    for (std::size_t index = 0; index < _allSessions.size(); ++index) {
      callback(_allSessions[index]);
    }

  }
}
