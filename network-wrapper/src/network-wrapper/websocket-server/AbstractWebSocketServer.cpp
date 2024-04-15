
#include "AbstractWebSocketServer.hpp"

#include "internals/WebSocketServer.hpp"

std::unique_ptr<AbstractWebSocketServer>
AbstractWebSocketServer::create(const std::string& ipAddress, uint16_t port, uint32_t totalThreads /*= 1*/) {
  return std::make_unique<WebSocketServer>(ipAddress, port, totalThreads);
}
