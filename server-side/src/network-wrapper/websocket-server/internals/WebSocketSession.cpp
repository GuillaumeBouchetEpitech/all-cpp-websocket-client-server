
#include "WebSocketSession.hpp"

#include <iostream>

//------------------------------------------------------------------------------

namespace {

// Report a failure
void
fail(beast::error_code ec, char const* what) {
  std::cerr << what << ": " << ec.message() << "\n";
}

} // namespace

SendBuffer::SendBuffer(const char* dataToSend, std::size_t dataSize)
  : size(dataSize) {
  if (dataSize > max_send_buffer_size) {
    throw std::runtime_error("send buffer requested size is too big");
  }

  std::memcpy(data, dataToSend, dataSize);
}

//
//
//
//
//

// Take ownership of the socket
WebSocketSession::WebSocketSession(
  boost::asio::ip::tcp::socket&& socket,
    const ws_callbacks::OnConnection& onConnectionCallback,
    const ws_callbacks::OnDisconnection& onDisconnectionCallback,
    const ws_callbacks::OnMessage& onMessageCallback
  )
  : _ws(std::move(socket))
  , _onConnectionCallback(onConnectionCallback)
  , _onDisconnectionCallback(onDisconnectionCallback)
  , _onMessageCallback(onMessageCallback)
  {}

// Get on the correct executor
void
WebSocketSession::run() {

  // allow shared ownership to net::dispatch callback
  auto self = shared_from_this();

  // We need to be executing within a strand to perform async operations
  // on the I/O objects in this session. Although not strictly necessary
  // for single-threaded contexts, this example code is written to be
  // thread-safe by default.
  net::dispatch(
    _ws.get_executor(),
    beast::bind_front_handler(&WebSocketSession::_onRun, self));
}

void
WebSocketSession::write(const char* data, std::size_t length) {
  const bool wasEmpty = _buffersToSend.empty();

  _buffersToSend.push_back(SendBuffer(data, length));

  if (wasEmpty == true) {

    // allow shared ownership to async_write callback
    auto self = shared_from_this();

    const SendBuffer& buffer = _buffersToSend.front();

    _ws.async_write(
      boost::asio::buffer(buffer.data, buffer.size),
      beast::bind_front_handler(&WebSocketSession::_onWrite, self));
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
  _ws.set_option(
    websocket::stream_base::timeout::suggested(beast::role_type::server));

  // Set a decorator to change the Server of the handshake
  _ws.set_option(
    websocket::stream_base::decorator([](websocket::response_type& res) {
      res.set(
        http::field::server,
        std::string(BOOST_BEAST_VERSION_STRING) + " websocket-server-async");
    }));

  // allow shared ownership to async_accept callback
  auto self = shared_from_this();

  // Accept the websocket handshake
  _ws.async_accept(
    beast::bind_front_handler(&WebSocketSession::_onAccept, self));
}

void
WebSocketSession::_onAccept(beast::error_code ec) {

  if (ec) {
    return fail(ec, "accept");
  }

  if (_onConnectionCallback) {

    // allow shared ownership to _onConnectionCallback
    auto self = shared_from_this();

    _onConnectionCallback(self);
  }

  // Read a message
  _doRead();
}

void
WebSocketSession::_doRead() {

  // allow shared ownership to async_read callback
  auto self = shared_from_this();

  // Read a message into our buffer
  _ws.async_read(
    _buffer, beast::bind_front_handler(&WebSocketSession::_onRead, self));
}

void
WebSocketSession::_onRead(beast::error_code ec, std::size_t bytes_transferred) {
  boost::ignore_unused(bytes_transferred);

  // This indicates that the session was closed
  if (ec == websocket::error::closed) {
    if (_onDisconnectionCallback) {

      // allow shared ownership to _onDisconnectionCallback
      auto self = shared_from_this();

      _onDisconnectionCallback(self);
    }

    return;
  }

  if (ec) {
    return fail(ec, "read");
  }

  if (_onMessageCallback) {
    const auto& subBuffer = _buffer.data();
    const char* dataPtr = static_cast<const char*>(subBuffer.data());
    const std::size_t dataLength = subBuffer.size();

    // allow shared ownership to _onMessageCallback
    auto self = shared_from_this();

    _onMessageCallback(self, dataPtr, dataLength);
  }

  // Clear the buffer
  _buffer.consume(_buffer.size());

  // Do another read
  _doRead();
}

void
WebSocketSession::_onWrite(
  beast::error_code ec, std::size_t bytes_transferred) {
  boost::ignore_unused(bytes_transferred);

  if (ec) {
    return fail(ec, "write");
  }

  if (_buffersToSend.size() > 1) {
    _buffersToSend.pop_front();

    const SendBuffer& buffer = _buffersToSend.front();

    // allow shared ownership to async_write callback
    auto self = shared_from_this();

    _ws.async_write(
      boost::asio::buffer(buffer.data, buffer.size),
      beast::bind_front_handler(&WebSocketSession::_onWrite, self));
  } else {
    _buffersToSend.pop_front();
  }
}
