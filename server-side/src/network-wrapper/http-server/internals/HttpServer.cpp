
#include "HttpServer.hpp"

#include <memory>

HttpServer::HttpServer(
  const std::string& ipAddress, const uint16_t port,
  const uint32_t totalThreads /*= 1*/)
  : _ioc(totalThreads),
    _acceptor(_ioc, {net::ip::make_address(ipAddress), port}),
    _tcpSocket(_ioc), _totalThreads(totalThreads) {
  if (totalThreads == 0) {
    throw std::runtime_error("total thread(s) must be > 0");
  }
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

  _doAccept();

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

  _acceptor.close();
  _ioc.stop();

  for (std::size_t index = 0; index < _allThreads.size(); ++index) {

    auto& currThread = _allThreads.at(index);

    if (currThread.joinable()) {
      currThread.join();
    }
  }
  _allThreads.clear();
}

void
HttpServer::_doAccept() {

  // allow shared ownership to async_accept callback
  auto self = shared_from_this();

  _acceptor.async_accept(
    // The new connection gets its own strand
    net::make_strand(_ioc),
    [self](beast::error_code ec, boost::asio::ip::tcp::socket newSocket) {

      if (!ec) {
        // could be stored, but will handle it own lifecycle in any case
        auto newClient = std::make_shared<HttpConnection>(std::move(newSocket));
        newClient->setOnConnectionCallback(self->_onConnectionCallback);
        newClient->start();
      }

      // Accept another connection
      self->_doAccept();
    });
}
