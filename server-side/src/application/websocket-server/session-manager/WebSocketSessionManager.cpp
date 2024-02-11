
#include "WebSocketSessionManager.hpp"

#include <mutex>

namespace {

template <typename T>
bool
_no_realloc_erase(std::vector<T>& container, const T& value) {
  auto it = std::find(container.begin(), container.end(), value);
  if (it == container.end()) {
    return false;
  }

  // the value to remove from the container was found

  // is the value the last element of the container?
  const bool isLastElement = (it == (container.end() - 1));
  if (!isLastElement) {
    // not the last element -> swap with the last element of the container
    std::swap(*it, container.back());
  }

  // remove the (potentially swapped) value from the container
  // this will not reallocate
  // -> unlike std::vector<T>::erase()
  container.pop_back();
  return true;
}

} // namespace

WebSocketSessionManager::WebSocketSessionManager() {
  _allSessions.reserve(1024);
}

void
WebSocketSessionManager::addSession(SessionPtr wsSession) {

  // only one write at a time
  std::unique_lock uniqueLock(_mutex);

  _allSessions.push_back(wsSession);
}

void
WebSocketSessionManager::removeSession(SessionPtr wsSession) {

  // only one write at a time
  std::unique_lock uniqueLock(_mutex);

  const bool wasRemoved = _no_realloc_erase(_allSessions, wsSession);

  if (!wasRemoved) {
    throw std::runtime_error("websocket session to remove was not found");
  }
}

void
WebSocketSessionManager::forEachSession(
  const std::function<void(SessionPtr)>& callback) {

  // allow multiple read at the same time
  std::shared_lock sharedLock(_mutex);

  for (std::size_t index = 0; index < _allSessions.size(); ++index) {
    callback(_allSessions[index]);
  }
}

void
WebSocketSessionManager::forEachSession(
  const std::function<void(SessionPtr)>& callback) const {

  // allow multiple read at the same time
  std::shared_lock sharedLock(_mutex);

  for (std::size_t index = 0; index < _allSessions.size(); ++index) {
    callback(_allSessions[index]);
  }
}
