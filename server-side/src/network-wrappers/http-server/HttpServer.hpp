
#pragma once

#include "internal/HttpConnection.hpp"
#include "internal/boostHeaders.hpp"

#include <vector>

class HttpServer {

public:
  using OnConnectionCallback = HttpConnection::OnConnectionCallback;

private:
  net::io_context _ioc;
  tcp::acceptor _acceptor;
  tcp::socket _socket;
  uint32_t _totalThreads;

private:
  OnConnectionCallback _onConnectionCallback = nullptr;
  std::vector<std::thread> _allThreads;

public:
  HttpServer(
    const boost::asio::ip::address inAddress, const uint16_t inPort,
    const uint32_t inTotalThreads = 1);

  HttpServer(const HttpServer& other) = delete;
  HttpServer(HttpServer&& other) = delete;
  HttpServer& operator=(const HttpServer& other) = delete;
  HttpServer& operator=(HttpServer&& other) = delete;

  ~HttpServer();

public:
  void setOnConnectionCallback(OnConnectionCallback onRequestCallback);

private:
  void _doAccept();

public:
  void start();
  void stop();
};
