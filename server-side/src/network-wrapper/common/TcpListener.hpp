
#pragma once

#include "boostHeaders.hpp"

#include <functional>
#include <memory>
#include <vector>

class AbstractWebSocketSession;
class WebSocketSession;

using OnNewConnection = std::function<void(boost::asio::ip::tcp::socket&&)>;

// Accepts incoming connections and launches the sessions
class TcpListener : public std::enable_shared_from_this<TcpListener> {
  friend WebSocketSession;

public:
  TcpListener(
    net::io_context& ioc,
    boost::asio::ip::tcp::endpoint endpoint,
    uint32_t totalThreads);

  ~TcpListener();

public:
  void setOnNewConnectionCallback(const OnNewConnection& onNewConnectionCallback);
  void start();
  void stop();

private:
  void _doAccept();

private:
  net::io_context& _ioc;
  boost::asio::ip::tcp::acceptor _acceptor;
  boost::asio::ip::tcp::endpoint _endpoint;
  uint32_t _totalThreads = 0;

  OnNewConnection _onNewConnectionCallback;
};
