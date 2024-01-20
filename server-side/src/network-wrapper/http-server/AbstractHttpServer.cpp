
#include "AbstractHttpServer.hpp"

#include "internals/HttpServer.hpp"

std::shared_ptr<AbstractHttpServer>
AbstractHttpServer::create(
  const std::string& inIpAddress, uint16_t inPort,
  uint32_t inTotalThreads /*= 1*/
) {
  return std::make_shared<HttpServer>(inIpAddress, inPort, inTotalThreads);
}