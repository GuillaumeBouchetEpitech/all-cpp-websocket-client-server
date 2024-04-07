
#pragma once

#include "../AbstractTcpListener.hpp"

#include "boostHeaders.hpp"

#include <functional>
#include <memory>
#include <vector>

class AbstractWebSocketSession;
class WebSocketSession;

// Accepts incoming connections and launches the sessions
class TcpListener : public AbstractTcpListener, public std::enable_shared_from_this<TcpListener> {
  friend WebSocketSession;

public:
  TcpListener(net::io_context& ioc, boost::asio::ip::tcp::endpoint endpoint, bool useBoostStrands);
  ~TcpListener();

  TcpListener(const TcpListener& other) = delete;
  TcpListener(TcpListener&& other) = delete;
  TcpListener& operator=(const TcpListener& other) = delete;
  TcpListener& operator=(TcpListener&& other) = delete;

public:
  void setOnNewConnectionCallback(const OnNewConnection& onNewConnectionCallback) override;
  void start() override;
  void stop() override;

private:
  void _doAccept();

private:
  net::io_context& _ioc;
  boost::asio::ip::tcp::acceptor _acceptor;
  boost::asio::ip::tcp::endpoint _endpoint;
  bool _useBoostStrands = false;

  OnNewConnection _onNewConnectionCallback;
};
