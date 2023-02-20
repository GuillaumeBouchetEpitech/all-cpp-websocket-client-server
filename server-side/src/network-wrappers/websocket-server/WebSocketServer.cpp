
#include "WebSocketServer.hpp"

WebSocketServer::WebSocketServer(
  boost::asio::ip::address inAddress, uint16_t inPort, uint32_t inTotalThreads)
  : _ioc(inTotalThreads), _totalThreads(inTotalThreads) {
  if (inTotalThreads == 0)
    throw std::runtime_error("total thread(s) must be > 0");

  Sessions allSessions;
  allSessions.reserve(1024);

  // Create and launch a listening port
  _mainTcpListener =
    std::make_shared<TcpListener>(_ioc, tcp::endpoint{inAddress, inPort});
}

WebSocketServer::~WebSocketServer() { stop(); }

void
WebSocketServer::setOnConnectionCallback(
  OnConnectionCallback onConnectionCallback) {
  _mainTcpListener->setOnConnectionCallback(onConnectionCallback);
}

void
WebSocketServer::setOnDisconnectionCallback(
  OnDisconnectionCallback onDisconnectionCallback) {
  _mainTcpListener->setOnDisconnectionCallback(onDisconnectionCallback);
}

void
WebSocketServer::setOnMessageCallback(OnMessageCallback onMessageCallback) {
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
    _allThreads.emplace_back([this] { _ioc.run(); });
  // _ioc.run();
}

void
WebSocketServer::stop() {
  if (_allThreads.empty())
    return;

  _mainTcpListener->stop();
  _ioc.stop();

  for (std::size_t index = 0; index < _allThreads.size(); ++index)
    if (_allThreads[index].joinable())
      _allThreads[index].join();
  _allThreads.clear();
}
