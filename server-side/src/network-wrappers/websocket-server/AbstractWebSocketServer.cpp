
#include "AbstractWebSocketServer.hpp"

#include "internals/WebSocketServer.hpp"

std::unique_ptr<AbstractWebSocketServer>
AbstractWebSocketServer::create(
  const std::string& inIpAddress, uint16_t inPort,
  uint32_t inTotalThreads /*= 1*/)
{
  return std::make_unique<WebSocketServer>(inIpAddress, inPort, inTotalThreads);
}

