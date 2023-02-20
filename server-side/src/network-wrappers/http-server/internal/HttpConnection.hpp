
#pragma once

#include "./boostHeaders.hpp"

#include <functional>

class HttpConnection : public std::enable_shared_from_this<HttpConnection> {
public:
  HttpConnection(tcp::socket socket);

public:
  using OnConnectionCallback = std::function<void(
    const http::request<http::dynamic_body>&,
    http::response<http::dynamic_body>&)>;

private:
  OnConnectionCallback _onConnectionCallback;

public:
  HttpConnection&
  setOnConnectionCallback(OnConnectionCallback inOnConnectionCallback);

public:
  // Initiate the asynchronous operations associated with the connection.
  void start();

private:
  // The socket for the currently connected client.
  tcp::socket _socket;

  // The buffer for performing reads.
  beast::flat_buffer _buffer{8192};

  // The request message.
  http::request<http::dynamic_body> _request;

  // The response message.
  http::response<http::dynamic_body> _response;

private:
  // The timer for putting a deadline on connection processing.
  net::steady_timer _deadline{_socket.get_executor(), std::chrono::seconds(60)};

  // Asynchronously receive a complete request message.
  void read_request();

  // Determine what needs to be done with the request message.
  void process_request();

  // // Construct a response message based on the program state.
  // void create_response();

  // Asynchronously transmit the response message.
  void write_response();

  // Check whether we have spent enough time on this connection.
  void check_deadline();
};
