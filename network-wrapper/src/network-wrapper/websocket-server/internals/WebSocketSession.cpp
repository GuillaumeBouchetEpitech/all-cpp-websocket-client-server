
#include "WebSocketSession.hpp"

#include <iostream>

//------------------------------------------------------------------------------

WebSocketSession::SendBuffer::SendBuffer(const char* dataToSend, std::size_t dataSize) {
  if (dataSize == 0) {
    throw std::runtime_error("send buffer requested size cannot be 0");
  }

  this->size = dataSize;
  this->data = std::make_unique<char[]>(dataSize);

  std::memcpy(this->data.get(), dataToSend, dataSize);
}

//
//
//
//
//

// Take ownership of the socket
WebSocketSession::WebSocketSession(
  boost::asio::ip::tcp::socket&& socket,
  bool useBoostStrands,
  std::shared_ptr<net::strand<net::any_io_executor>> strand,
  const ws_callbacks::AllCallbacks& allCallbacks)
  : _ws(std::move(socket)), _strand(strand), _useBoostStrands(useBoostStrands), _allCallbacks(allCallbacks) {}

WebSocketSession::~WebSocketSession() { disconnect(); }

// Get on the correct executor
void
WebSocketSession::run() {

  // allow shared ownership to net::post callback
  auto self = shared_from_this();

  // We need to be executing within a strand to perform async operations
  // on the I/O objects in this session. Although not strictly necessary
  // for single-threaded contexts, this example code is written to be
  // thread-safe by default.
  if (_useBoostStrands) {
    // must start on the strand from this point
    net::post(*_strand, beast::bind_front_handler(&WebSocketSession::_onRun, self));
  } else {
    _onRun();
  }
}

void
WebSocketSession::disconnect() {
  if (!_isConnected) {
    return;
  }
  _isConnected = false;
  _ws.close(websocket::close_code::normal);
}

void
WebSocketSession::write(const char* data, std::size_t length) {

  const std::size_t newSize = _sendBufferSafeQueue.emplaceBack(data, length);

  if (newSize > 1) {
    // assume the next _onWrite callback will pick it up
    return;
  }

  if (_useBoostStrands) {
    // start it on the strand
    net::post(*_strand, [this] { this->_doWrite(); });
  } else {
    _doWrite();
  }
}

//
//
//
//
//

// Start the asynchronous operation
void
WebSocketSession::_onRun() {
  // Set suggested timeout settings for the websocket
  _ws.set_option(websocket::stream_base::timeout::suggested(beast::role_type::server));

  // Set a decorator to change the Server of the handshake
  _ws.set_option(websocket::stream_base::decorator([](websocket::response_type& res) {
    res.set(http::field::server, std::string(BOOST_BEAST_VERSION_STRING) + " websocket-server-async");
  }));

  // Accept the websocket handshake
  _ws.async_accept(beast::bind_front_handler(&WebSocketSession::_onAccept, shared_from_this()));
}

void
WebSocketSession::_onAccept(beast::error_code ec) {

  auto self = shared_from_this();

  if (ec) {
    return _connectionError(ec, "accept");
  }

  if (_allCallbacks.onConnection) {
    _allCallbacks.onConnection(self);
  }

  // Read a message
  if (_useBoostStrands) {
    // start it on the strand
    net::post(*_strand, [self] { self->_doRead(); });
  } else {
    _doRead();
  }
}

void
WebSocketSession::_doRead() {

  auto self = shared_from_this();

  // Read a message into our buffer
  _ws.async_read(_readBuffer, beast::bind_front_handler(&WebSocketSession::_onRead, self));
}

void
WebSocketSession::_onRead(beast::error_code ec, std::size_t bytes_transferred) {
  boost::ignore_unused(bytes_transferred);

  // This indicates that the session was closed
  if (ec) {
    return _connectionError(ec, "read");
  }

  if (_allCallbacks.onMessage) {
    const auto& subBuffer = _readBuffer.data();
    const char* dataPtr = static_cast<const char*>(subBuffer.data());
    const std::size_t dataLength = subBuffer.size();

    _allCallbacks.onMessage(shared_from_this(), dataPtr, dataLength);
  }

  // Clear the buffer
  _readBuffer.consume(_readBuffer.size());

  // Do another read
  _doRead();
}

void
WebSocketSession::_onWrite(beast::error_code ec, std::size_t bytes_transferred) {
  boost::ignore_unused(bytes_transferred);

  if (ec) {
    return _connectionError(ec, "write");
  }

  _sendBufferSafeQueue.popFront();
  if (_sendBufferSafeQueue.empty() == false) {
    _doWrite();
  }
}

void
WebSocketSession::_doWrite() {
  const SendBuffer& buffer = _sendBufferSafeQueue.peekFront();

  _ws.async_write(
    boost::asio::buffer(buffer.data.get(), buffer.size),
    beast::bind_front_handler(&WebSocketSession::_onWrite, shared_from_this()));
}

void
WebSocketSession::_connectionError(beast::error_code ec, char const* what) {
  std::cerr << "WebSocketSession:failure \"" << what << "\": \"" << ec.message() << "\"" << std::endl;
  // if (_onErrorCallback) {
  //   _onErrorCallback(ec.message());
  // }

  // alternative disconnect
  _isConnected = false;

  if (_allCallbacks.onDisconnection) {
    _allCallbacks.onDisconnection(shared_from_this());
  }
}
