
#include "AbstractHttpServer.hpp"

#include "internals/HttpServer.hpp"

std::shared_ptr<AbstractHttpServer>
AbstractHttpServer::create(
  const std::string& ip_address,
  uint16_t port,
  uint32_t total_threads /*= 1*/
) {
  return std::make_shared<HttpServer>(ip_address, port, total_threads);
}
