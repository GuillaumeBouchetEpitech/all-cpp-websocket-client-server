
#include "WebSocketSessionManager.hpp"

namespace {

template<typename T>
bool _no_realloc_erase(std::vector<T>& inContainer, const T& inValue)
{
  auto it = std::find(inContainer.begin(), inContainer.end(), inValue);
  if (it == inContainer.end()) {
    return false;
  }

  // the value to remove from the container was found

  // is the value the last element of the container?
  const bool isLastElement = (it == (inContainer.end() - 1));
  if (!isLastElement) {
    // not the last element -> swap with the last element of the container
    std::swap(*it, inContainer.back());
  }

  // remove the (potentially swapped) value from the container
  // this will not reallocate
  // -> unlike std::vector<T>::erase()
  inContainer.pop_back();
  return true;
}

}

WebSocketSessionManager::WebSocketSessionManager() {
  _allSessions.reserve(1024);
}

void
WebSocketSessionManager::addSession(SessionPtr inWsSession) {

  // only one write at a time
  std::unique_lock uniqueLock(_mutex);

  _allSessions.push_back(inWsSession);
}

void
WebSocketSessionManager::removeSession(SessionPtr inWsSession) {

  // only one write at a time
  std::unique_lock uniqueLock(_mutex);

  const bool wasRemoved = _no_realloc_erase(_allSessions, inWsSession);

  if (!wasRemoved) {
    throw std::runtime_error("websocket session to remove was not found");
  }
}

void
WebSocketSessionManager::forEachSession(
  const std::function<void(SessionPtr)>& inCallback) {

  // allow multiple read at the same time
  std::shared_lock sharedLock(_mutex);

  for (std::size_t index = 0; index < _allSessions.size(); ++index) {
    inCallback(_allSessions[index]);
  }
}
