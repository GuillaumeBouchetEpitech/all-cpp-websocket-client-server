
#include "WebSocketSessionManager.hpp"

WebSocketSessionManager::WebSocketSessionManager() {
  _allSessions.reserve(1024);
}

void
WebSocketSessionManager::addSession(
  std::shared_ptr<WebSocketSession> inWsSession) {
  std::unique_lock uniqueLock(_mutex); // only one write at a time

  _allSessions.push_back(inWsSession);
}

void
WebSocketSessionManager::removeSession(
  std::shared_ptr<WebSocketSession> inWsSession) {
  std::unique_lock uniqueLock(_mutex); // only one write at a time

  for (std::size_t index = 0; index < _allSessions.size(); ++index)
    if (_allSessions[index] == inWsSession) {
      const bool isLastElement = (index + 1 == _allSessions.size());

      if (!isLastElement) {
        // alternative to erase() -> will avoid a potential reallocation
        std::swap(_allSessions[index], _allSessions.back());
      }

      _allSessions.pop_back();
      break;
    }
}

void
WebSocketSessionManager::forEachSession(
  const std::function<void(std::shared_ptr<WebSocketSession>)>& inCallback) {
  std::shared_lock sharedLock(_mutex); // allow multiple read at the same time

  for (std::size_t index = 0; index < _allSessions.size(); ++index) {
    inCallback(_allSessions[index]);
  }
}
