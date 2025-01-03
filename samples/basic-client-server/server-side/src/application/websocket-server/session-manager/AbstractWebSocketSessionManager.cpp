
#include "AbstractWebSocketSessionManager.hpp"

#include "./internals/ThreadSafeWebSocketSessionManager.hpp"
#include "./internals/ThreadUnsafeWebSocketSessionManager.hpp"

AbstractWebSocketSessionManager::AbstractWebSocketSessionManager() { _allSessions.reserve(1024); }

std::shared_ptr<AbstractWebSocketSessionManager> AbstractWebSocketSessionManager::createThreadSafe()
{
  return std::make_shared<ThreadSafeWebSocketSessionManager>();
}
std::shared_ptr<AbstractWebSocketSessionManager> AbstractWebSocketSessionManager::createThreadUnsafe()
{
  return std::make_shared<ThreadUnsafeWebSocketSessionManager>();
}



