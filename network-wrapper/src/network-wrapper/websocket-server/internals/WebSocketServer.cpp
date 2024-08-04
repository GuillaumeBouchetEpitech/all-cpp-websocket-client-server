
#include "WebSocketServer.hpp"

#include "WebSocketSession.hpp"

#include "boostHeaders.hpp"

WebSocketServer::WebSocketServer(
  const std::string& ipAddress,
  uint16_t port,
  uint32_t totalThreads, /*= 1*/
  bool useStrands        /*= false*/
  )
  : _ioc(totalThreads), _sharedStrand(std::make_shared<net::strand<net::any_io_executor>>(net::make_strand(_ioc))),
    _totalThreads(totalThreads), _useStrands(useStrands) {

  if (totalThreads == 0) {
    throw std::runtime_error("total thread(s) must be > 0");
  }

  const bool useBoostStrands = false; // the tcp listener doesn't need strands
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
    const bool useBoostStrands = (_totalThreads > 1 && _useStrands);

    // newSocket.rate_policy().read_limit(10000); // bytes per second
    // newSocket.rate_policy().write_limit(850000); // bytes per second

    // Create the session and run it
    // -> the session is not stored anywhere
    // -> since calling run() will make more shared pointer of the instance
    //    the session will not be deleted when going out of scope here...
    //    ...if anything it's just a bit misleading, hence that comment
    auto newSession = std::make_shared<WebSocketSession>(
      std::move(newSocket),
      useBoostStrands,
      _sharedStrand,
      _onConnectionCallback,
      _onDisconnectionCallback,
      _onMessageCallback);

    newSession->run();
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
