
#include "AbstractWebSocketServer.hpp"

#include "internals/WebSocketServer.hpp"

std::shared_ptr<AbstractWebSocketServer>
AbstractWebSocketServer::create(
  const std::string& ipAddress, uint16_t port, uint32_t totalThreads /*= 1*/, bool useStrands /*= false*/) {
  return std::make_shared<WebSocketServer>(ipAddress, port, totalThreads, useStrands);
}
