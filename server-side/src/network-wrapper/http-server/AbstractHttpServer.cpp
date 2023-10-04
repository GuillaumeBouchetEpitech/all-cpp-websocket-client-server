
#include "AbstractHttpServer.hpp"

#include "internals/HttpServer.hpp"

std::unique_ptr<AbstractHttpServer>
AbstractHttpServer::create(
  const std::string& inIpAddress, uint16_t inPort,
  uint32_t inTotalThreads /*= 1*/
) {
  return std::make_unique<HttpServer>(inIpAddress, inPort, inTotalThreads);
}