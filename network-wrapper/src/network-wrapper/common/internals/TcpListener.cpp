
#include "TcpListener.hpp"

#include <iostream>

namespace {

// Report a failure
void
fail(beast::error_code ec, char const* what) {
  std::cerr << "TcpListener:failure \"" << what << "\": \"" << ec.message() << "\"" << std::endl;
}

} // namespace

TcpListener::TcpListener(net::io_context& ioc, boost::asio::ip::tcp::endpoint endpoint, bool useBoostStrands)
  : _ioc(ioc), _sharedStrand(net::make_strand(_ioc)), _acceptor(ioc), _endpoint(endpoint),
    _useBoostStrands(useBoostStrands) {}

TcpListener::~TcpListener() { stop(); }

//
//
//
//
//

void
TcpListener::setOnNewConnectionCallback(const OnNewConnection& onNewConnectionCallback) {
  _onNewConnectionCallback = onNewConnectionCallback;
}

// Start accepting incoming connections
void
TcpListener::start() {

  if (!_onNewConnectionCallback) {
    throw std::runtime_error("not callback(s) setup");
  }

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
  if (_acceptor.is_open()) {
    _acceptor.close();
  }
}

void
TcpListener::_doAccept() {

  // allow shared ownership to TcpListener::_onAccept
  auto self = shared_from_this();

  if (_useBoostStrands) {
    // more than one thread
    // -> the new connection gets its own boost::strand
    // -> it will scale well across the available threads
    _acceptor.async_accept(_sharedStrand, beast::bind_front_handler(&TcpListener::_onAccept, self));
  } else {
    // only one thread
    // -> then it's faster NOT to use boost::strand
    _acceptor.async_accept(_ioc, beast::bind_front_handler(&TcpListener::_onAccept, self));
  }
}

void
TcpListener::_onAccept(beast::error_code ec, boost::asio::ip::tcp::socket socket) {
  if (ec) {
    fail(ec, "accept");
    return;
  }

  _onNewConnectionCallback(std::move(socket));

  // Accept another connection
  _doAccept();
}
