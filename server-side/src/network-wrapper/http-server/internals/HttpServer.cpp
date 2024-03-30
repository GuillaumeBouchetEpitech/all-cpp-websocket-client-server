
#include "HttpServer.hpp"

#include <memory>

HttpServer::HttpServer(
  const std::string& ipAddress, const uint16_t port,
  const uint32_t totalThreads /*= 1*/)
  : _ioc(totalThreads),
    _totalThreads(totalThreads)
{
  if (totalThreads == 0) {
    throw std::runtime_error("total thread(s) must be > 0");
  }

  auto boostIpAddr = net::ip::make_address(ipAddress);
  auto boostEndpoint = boost::asio::ip::tcp::endpoint{boostIpAddr, port};

  // Create and launch a listening port
  _mainTcpListener = std::make_shared<TcpListener>(_ioc, boostEndpoint, _totalThreads);

}

HttpServer::~HttpServer() { stop(); }

void
HttpServer::setOnConnectionCallback(
  const http_callbacks::OnConnection& onConnectionCallback) {
  _onConnectionCallback = onConnectionCallback;
}

void
HttpServer::start() {

  if (!_onConnectionCallback) {
    throw std::runtime_error("missing OnConnectionCallback");
  }

  stop();

  auto self = shared_from_this();

  _mainTcpListener->setOnNewConnectionCallback([self](boost::asio::ip::tcp::socket&& newSocket)
  {

    // Create the http-connection and run it
    // -> the http-connection is not stored anywhere
    // -> since calling start() will make more shared pointer of the instance
    //    the http-connection will not be deleted when going out of scope here...
    //    ...if anything it's just a bit misleading, hence that comment
    auto newClient = std::make_shared<HttpConnection>(std::move(newSocket));
    newClient->setOnConnectionCallback(self->_onConnectionCallback);
    newClient->start();
  });

  _mainTcpListener->start();

  // Run the I/O service on the requested number of threads
  _allThreads.reserve(_totalThreads);
  for (uint32_t index = 0; index < _totalThreads; ++index) {
    _allThreads.emplace_back([this] { _ioc.run(); });
}
}

void
HttpServer::stop() {
  if (_allThreads.empty()) {
    return;
  }

  _mainTcpListener->stop();
  _ioc.stop();

  for (auto& currThread : _allThreads) {
    if (currThread.joinable()) {
      currThread.join();
    }
  }
  _allThreads.clear();
}
