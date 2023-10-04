
#include "WebSocketServer.hpp"

#include "boostHeaders.hpp"

WebSocketServer::WebSocketServer(
  const std::string& inIpAddress, uint16_t inPort,
  uint32_t inTotalThreads /*= 1*/
  )
  : _ioc(inTotalThreads), _totalThreads(inTotalThreads) {
  if (inTotalThreads == 0) {
    throw std::runtime_error("total thread(s) must be > 0");
  }

  // Create and launch a listening port
  _mainTcpListener = std::make_shared<TcpListener>(
    _ioc,
    boost::asio::ip::tcp::endpoint{net::ip::make_address(inIpAddress), inPort});
}

WebSocketServer::~WebSocketServer() { stop(); }

void
WebSocketServer::setOnConnectionCallback(
  const ws_callbacks::OnConnection& onConnectionCallback) {
  _mainTcpListener->setOnConnectionCallback(onConnectionCallback);
}

void
WebSocketServer::setOnDisconnectionCallback(
  const ws_callbacks::OnDisconnection& onDisconnectionCallback) {
  _mainTcpListener->setOnDisconnectionCallback(onDisconnectionCallback);
}

void
WebSocketServer::setOnMessageCallback(
  const ws_callbacks::OnMessage& onMessageCallback) {
  _mainTcpListener->setOnMessageCallback(onMessageCallback);
}

// void WebSocketServer::start() {

//   _mainTcpListener->run();

//   // Run the I/O service on the requested number of threads
//   std::vector<std::thread> allThreads;
//   allThreads.reserve(_totalThreads - 1);
//   for (uint32_t index = _totalThreads - 1; index > 0; --index)
//     allThreads.emplace_back([this] { _ioc.run(); });
//   _ioc.run();
// }

// void WebSocketServer::stop()
// {
// }

void
WebSocketServer::start() {
  stop();

  _mainTcpListener->start();

  // Run the I/O service on the requested number of threads
  _allThreads.reserve(_totalThreads);
  for (uint32_t index = 0; index < _totalThreads; ++index)
    _allThreads.emplace_back([this]() { _ioc.run(); });
  // _ioc.run();
}

void
WebSocketServer::stop() {
  if (_allThreads.empty()) {
    return;
  }

  _mainTcpListener->stop();
  _ioc.stop();

  for (std::size_t index = 0; index < _allThreads.size(); ++index) {
    if (_allThreads[index].joinable()) {
      _allThreads[index].join();
    }
  }
  _allThreads.clear();
}
