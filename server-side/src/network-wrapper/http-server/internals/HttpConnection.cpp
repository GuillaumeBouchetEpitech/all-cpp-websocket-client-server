
#include "HttpConnection.hpp"

HttpConnection::HttpConnection(boost::asio::ip::tcp::socket&& tcpSocket)
  : _tcpSocket(std::move(tcpSocket)) {}

HttpConnection&
HttpConnection::setOnConnectionCallback(
  const http_callbacks::OnConnection& onConnectionCallback) {
  _onConnectionCallback = onConnectionCallback;
  return *this;
}

// Initiate the asynchronous operations associated with the connection.
void
HttpConnection::start() {
  _readRequest();
  _checkConnectionTimeout();
}

// Asynchronously receive a complete request message.
void
HttpConnection::_readRequest() {

  // allow shared ownership to async_read callback
  auto self = shared_from_this();

  http::async_read(
    _tcpSocket, _readBuffer, _requestBody,
    [self](beast::error_code ec, std::size_t bytes_transferred) {
      boost::ignore_unused(bytes_transferred);
      if (!ec) {
        self->_processRequest();
      }
    });
}

// Determine what needs to be done with the request message.
void
HttpConnection::_processRequest() {

  if (_onConnectionCallback) {
    _onConnectionCallback(_requestBody, _responseBody);
  }

  _writeResponse();
}

// Asynchronously transmit the response message.
void
HttpConnection::_writeResponse() {

  // allow shared ownership to async_write callback
  auto self = shared_from_this();

  _responseBody.content_length(_responseBody.body().size());

  http::async_write(
    _tcpSocket, _responseBody, [self](beast::error_code ec, std::size_t) {
      self->_tcpSocket.shutdown(
        boost::asio::ip::tcp::socket::shutdown_send, ec);
      self->_connectionTimer.cancel();
    });
}

void
HttpConnection::_checkConnectionTimeout() {

  // allow shared ownership to async_wait callback
  auto self = shared_from_this();

  _connectionTimer.async_wait([self](beast::error_code ec) {
    if (!ec) {
      // timeout -> close socket
      self->_tcpSocket.close(ec);
    }
  });
}
