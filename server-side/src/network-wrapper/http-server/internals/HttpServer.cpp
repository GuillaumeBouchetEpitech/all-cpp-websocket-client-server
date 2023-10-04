
#include "HttpServer.hpp"

#include <memory>

HttpServer::HttpServer(
  const std::string& inIpAddress, const uint16_t inPort,
  const uint32_t inTotalThreads /*= 1*/)
  : _ioc(inTotalThreads), _acceptor(_ioc, {net::ip::make_address(inIpAddress), inPort}), _socket(_ioc),
    _totalThreads(inTotalThreads) {
  if (inTotalThreads == 0)
    throw std::runtime_error("total thread(s) must be > 0");
}

HttpServer::~HttpServer() { stop(); }

void
HttpServer::setOnConnectionCallback(const http_callbacks::OnConnection& onConnectionCallback) {
  _onConnectionCallback = onConnectionCallback;
}

void
HttpServer::start() {
  if (!_onConnectionCallback)
    throw std::runtime_error("missing OnConnectionCallback");

  stop();

  _doAccept();

  // Run the I/O service on the requested number of threads
  _allThreads.reserve(_totalThreads);
  for (uint32_t index = 0; index < _totalThreads; ++index)
    _allThreads.emplace_back([this] { _ioc.run(); });
  // _ioc.run();
}

void
HttpServer::stop() {
  if (_allThreads.empty())
    return;

  _acceptor.close();
  _ioc.stop();

  for (std::size_t index = 0; index < _allThreads.size(); ++index)
    if (_allThreads[index].joinable())
      _allThreads[index].join();
  _allThreads.clear();
}

void
HttpServer::_doAccept() {
  _acceptor.async_accept(_socket, [this](beast::error_code ec) {
    if (!ec)
      std::make_shared<HttpConnection>(std::move(_socket))
        ->setOnConnectionCallback(_onConnectionCallback)
        .start();
    _doAccept();
  });
}
