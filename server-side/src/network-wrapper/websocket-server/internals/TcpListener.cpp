
#include "TcpListener.hpp"

#include "WebSocketSession.hpp"

#include <iostream>

namespace {

// Report a failure
void
fail(beast::error_code ec, char const* what) {
  std::cerr << what << ": " << ec.message() << "\n";
}

} // namespace

TcpListener::TcpListener(
  net::io_context& ioc, boost::asio::ip::tcp::endpoint endpoint)
  : _ioc(ioc), _acceptor(ioc), _endpoint(endpoint) {}

//
//
//
//
//

void
TcpListener::setOnConnectionCallback(
  const ws_callbacks::OnConnection& onConnectionCallback) {
  _onConnectionCallback = onConnectionCallback;
}

void
TcpListener::setOnDisconnectionCallback(
  const ws_callbacks::OnDisconnection& onDisconnectionCallback) {
  _onDisconnectionCallback = onDisconnectionCallback;
}

void
TcpListener::setOnMessageCallback(
  const ws_callbacks::OnMessage& onMessageCallback) {
  _onMessageCallback = onMessageCallback;
}

//
//
//
//
//

// Start accepting incoming connections
void
TcpListener::start() {

  if (
    !_onConnectionCallback && !_onDisconnectionCallback && !_onMessageCallback)
    throw std::runtime_error("not callback(s) setup");

  stop();

  beast::error_code ec;

  // Open the acceptor
  _acceptor.open(_endpoint.protocol(), ec);
  if (ec) {
    fail(ec, "open");
    return;
  }

  // Allow address reuse
  _acceptor.set_option(net::socket_base::reuse_address(true), ec);
  if (ec) {
    fail(ec, "set_option");
    return;
  }

  // Bind to the server address
  _acceptor.bind(_endpoint, ec);
  if (ec) {
    fail(ec, "bind");
    return;
  }

  // Start listening for connections
  _acceptor.listen(net::socket_base::max_listen_connections, ec);
  if (ec) {
    fail(ec, "listen");
    return;
  }

  _doAccept();
}

void
TcpListener::stop() {
  if (_acceptor.is_open())
    _acceptor.close();
}

void
TcpListener::_doAccept() {

  // allow shared ownership to async_accept callback
  auto self = shared_from_this();

  _acceptor.async_accept(
    // The new connection gets its own strand
    net::make_strand(_ioc),
    beast::bind_front_handler(&TcpListener::_onAccept, self));
}

void
TcpListener::_onAccept(
  beast::error_code ec, boost::asio::ip::tcp::socket socket) {

  if (ec) {
    fail(ec, "accept");
    return;
  }

  // Create the session and run it
  std::make_shared<WebSocketSession>(std::move(socket), *this)->run();

  // Accept another connection
  _doAccept();
}
