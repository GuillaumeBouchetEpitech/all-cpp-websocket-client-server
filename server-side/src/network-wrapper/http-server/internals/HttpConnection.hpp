
#pragma once

#include "../callbacks.hpp"

#include "./boostHeaders.hpp"

#include <functional>
#include <memory>

class HttpConnection : public std::enable_shared_from_this<HttpConnection> {
public:
  HttpConnection(boost::asio::ip::tcp::socket&& socket);

public:
  HttpConnection& setOnConnectionCallback(
    const http_callbacks::OnConnection& inOnConnectionCallback);

public:
  // Initiate the asynchronous operations associated with the connection.
  void start();

private:
  // Asynchronously receive a complete request message.
  void _read_request();

  // Determine what needs to be done with the request message.
  void _process_request();

  // // Construct a response message based on the program state.
  // void create_response();

  // Asynchronously transmit the response message.
  void _write_response();

  // Check whether we have spent enough time on this connection.
  void _check_deadline();

private:
  // The socket for the currently connected client.
  boost::asio::ip::tcp::socket _socket;

  // The buffer for performing reads.
  beast::flat_buffer _buffer{8192};

  // The request message.
  http::request<http::dynamic_body> _request;

  // The response message.
  http::response<http::dynamic_body> _response;

  // The timer for putting a deadline on connection processing.
  net::steady_timer _deadline{_socket.get_executor(), std::chrono::seconds(60)};

  http_callbacks::OnConnection _onConnectionCallback;
};
