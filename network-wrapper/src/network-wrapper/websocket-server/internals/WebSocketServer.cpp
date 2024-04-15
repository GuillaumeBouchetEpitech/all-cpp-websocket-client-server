
#include "WebSocketServer.hpp"

#include "WebSocketSession.hpp"

#include "boostHeaders.hpp"

WebSocketServer::WebSocketServer(
  const std::string& ipAddress, uint16_t port, uint32_t totalThreads /*= 1*/
  )
  : _ioc(totalThreads), _totalThreads(totalThreads) {
  if (totalThreads == 0) {
    throw std::runtime_error("total thread(s) must be > 0");
  }

  // more than one thread
  // -> the new connection gets its own boost::strand
  // -> it will scale well across the available threads
  // only one thread
  // -> then it's faster NOT to use boost::strand
  const bool useBoostStrands = (_totalThreads > 1);

  const auto boostIpAddr = net::ip::make_address(ipAddress);
  const auto boostEndpoint = boost::asio::ip::tcp::endpoint{boostIpAddr, port};

  _mainTcpListener = AbstractTcpListener::create(_ioc, boostEndpoint, useBoostStrands);
}

WebSocketServer::~WebSocketServer() { stop(); }

void
WebSocketServer::setOnConnectionCallback(const ws_callbacks::OnConnection& onConnectionCallback) {
  _onConnectionCallback = onConnectionCallback;
}

void
WebSocketServer::setOnDisconnectionCallback(const ws_callbacks::OnDisconnection& onDisconnectionCallback) {
  _onDisconnectionCallback = onDisconnectionCallback;
}

void
WebSocketServer::setOnMessageCallback(const ws_callbacks::OnMessage& onMessageCallback) {
  _onMessageCallback = onMessageCallback;
}

void
WebSocketServer::start() {
  stop();

  _mainTcpListener->setOnNewConnectionCallback([this](boost::asio::ip::tcp::socket&& newSocket) {
    // Create the session and run it
    // -> the session is not stored anywhere
    // -> since calling run() will make more shared pointer of the instance
    //    the session will not be deleted when going out of scope here...
    //    ...if anything it's just a bit misleading, hence that comment
    std::make_shared<WebSocketSession>(
      std::move(newSocket), _onConnectionCallback, _onDisconnectionCallback, _onMessageCallback)
      ->run();
  });

  _mainTcpListener->start();

  // Run the I/O service on the requested number of threads
  _allThreads.reserve(_totalThreads);
  for (uint32_t index = 0; index < _totalThreads; ++index) {
    _allThreads.emplace_back([this]() { _ioc.run(); });
  }
}

void
WebSocketServer::stop() {
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
