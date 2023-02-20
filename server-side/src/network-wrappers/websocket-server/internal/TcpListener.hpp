
#pragma once

#include "boostHeaders.hpp"

#include <memory>
#include <vector>

class WebSocketSession;

// Accepts incoming connections and launches the sessions
class TcpListener : public std::enable_shared_from_this<TcpListener> {
  friend WebSocketSession;

public:
  using OnConnectionCallback =
    std::function<void(std::shared_ptr<WebSocketSession>)>;
  using OnDisconnectionCallback = OnConnectionCallback;
  using OnMessageCallback = std::function<void(
    std::shared_ptr<WebSocketSession>, const beast::flat_buffer&)>;

private:
  net::io_context& _ioc;
  tcp::acceptor _acceptor;
  tcp::endpoint _endpoint;

  OnConnectionCallback _onConnectionCallback;
  OnDisconnectionCallback _onDisconnectionCallback;
  OnMessageCallback _onMessageCallback;

public:
  TcpListener(net::io_context& ioc, tcp::endpoint endpoint);

public:
  void setOnConnectionCallback(OnConnectionCallback onConnectionCallback);

  void
  setOnDisconnectionCallback(OnDisconnectionCallback onDisconnectionCallback);

  void setOnMessageCallback(OnMessageCallback onMessageCallback);

public:
  // Start accepting incoming connections
  void start();
  void stop();

private:
  void _doAccept();

  void _onAccept(beast::error_code ec, tcp::socket socket);
};
