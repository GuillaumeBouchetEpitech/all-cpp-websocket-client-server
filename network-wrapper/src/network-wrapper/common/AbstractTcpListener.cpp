
#include "AbstractTcpListener.hpp"

#include "./internals/TcpListener.hpp"

std::shared_ptr<AbstractTcpListener>
AbstractTcpListener::create(net::io_context& ioc, boost::asio::ip::tcp::endpoint endpoint, bool useBoostStrands) {
  return std::make_shared<TcpListener>(ioc, endpoint, useBoostStrands);
}
