
#pragma once

#include "../callbacks.hpp"

#include "./boostHeaders.hpp"

#include <functional>
#include <memory>

class HttpConnection : public std::enable_shared_from_this<HttpConnection> {
public:
  HttpConnection(boost::asio::ip::tcp::socket&& tcp_socket);

public:
  HttpConnection& setOnConnectionCallback(
    const http_callbacks::OnConnection& on_connection_callback);

public:
  // Initiate the asynchronous operations associated with the connection.
  void start();

private:
  // Asynchronously receive a complete request message.
  void _readRequest();

  // Determine what needs to be done with the request message.
  void _processRequest();

  // Asynchronously transmit the response message.
  void _writeResponse();

  // Check whether we have spent enough time on this connection.
  void _checkConnectionTimeout();

private:
  boost::asio::ip::tcp::socket _tcpSocket;
  beast::flat_buffer _readBuffer{8192};
  http::request<http::dynamic_body> _requestBody;
  http::response<http::dynamic_body> _responseBody;
  net::steady_timer _connectionTimer{_tcpSocket.get_executor(), std::chrono::seconds(60)};

  http_callbacks::OnConnection _onConnectionCallback;
};
