
#include "HttpConnection.hpp"

HttpConnection::HttpConnection(boost::asio::ip::tcp::socket socket)
  : _socket(std::move(socket)) {}

HttpConnection&
HttpConnection::setOnConnectionCallback(
  const http_callbacks::OnConnection& inOnConnectionCallback) {
  _onConnectionCallback = inOnConnectionCallback;
  return *this;
}

// Initiate the asynchronous operations associated with the connection.
void
HttpConnection::start() {
  _read_request();
  _check_deadline();
}

// Asynchronously receive a complete request message.
void
HttpConnection::_read_request() {
  auto self = shared_from_this();

  http::async_read(
    _socket, _buffer, _request,
    [self](beast::error_code ec, std::size_t bytes_transferred) {
      boost::ignore_unused(bytes_transferred);
      if (!ec)
        self->_process_request();
    });
}

// Determine what needs to be done with the request message.
void
HttpConnection::_process_request() {

  if (_onConnectionCallback) {
    _onConnectionCallback(_request, _response);
  }

  _write_response();
}

// Asynchronously transmit the response message.
void
HttpConnection::_write_response() {
  auto self = shared_from_this();

  _response.content_length(_response.body().size());

  http::async_write(
    _socket, _response, [self](beast::error_code ec, std::size_t) {
      self->_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_send, ec);
      self->_deadline.cancel();
    });
}

// Check whether we have spent enough time on this connection.
void
HttpConnection::_check_deadline() {
  auto self = shared_from_this();

  _deadline.async_wait([self](beast::error_code ec) {
    if (!ec) {
      // Close socket to cancel any outstanding operation.
      self->_socket.close(ec);
    }
  });
}
