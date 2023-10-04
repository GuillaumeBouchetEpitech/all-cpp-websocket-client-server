
#pragma once

#include "../callbacks.hpp"

#include "boostHeaders.hpp"

#include <memory>
#include <vector>
#include <functional>

class AbstractWebSocketSession;
class WebSocketSession;

// Accepts incoming connections and launches the sessions
class TcpListener : public std::enable_shared_from_this<TcpListener> {
  friend WebSocketSession;

public:
  TcpListener(net::io_context& ioc, boost::asio::ip::tcp::endpoint endpoint);

public:
  void setOnConnectionCallback(const ws_callbacks::OnConnection& onConnectionCallback);
  void setOnDisconnectionCallback(const ws_callbacks::OnDisconnection& onDisconnectionCallback);
  void setOnMessageCallback(const ws_callbacks::OnMessage& onMessageCallback);

public:
  // Start accepting incoming connections
  void start();
  void stop();

private:
  void _doAccept();

  void _onAccept(beast::error_code ec, boost::asio::ip::tcp::socket socket);

private:
  net::io_context& _ioc;
  boost::asio::ip::tcp::acceptor _acceptor;
  boost::asio::ip::tcp::endpoint _endpoint;

  ws_callbacks::OnConnection _onConnectionCallback;
  ws_callbacks::OnDisconnection _onDisconnectionCallback;
  ws_callbacks::OnMessage _onMessageCallback;

};
