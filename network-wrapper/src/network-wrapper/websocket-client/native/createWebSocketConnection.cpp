
#include "createWebSocketConnection.hpp"

#include "./internals/WebSocketConnection.hpp"

std::shared_ptr<AbstractWebSocketConnection>
createWebSocketConnection()
{
  return std::make_shared<WebSocketConnection>();
}
