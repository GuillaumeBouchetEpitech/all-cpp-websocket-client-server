
#pragma once

#include "../AbstractHttpServer.hpp"

#include "../callbacks.hpp"

#include "HttpConnection.hpp"
#include "boostHeaders.hpp"

#include <vector>

class HttpServer
  : public AbstractHttpServer
  , public std::enable_shared_from_this<HttpServer>
{

public:
  HttpServer(
    const std::string& inIpAddress, const uint16_t inPort,
    const uint32_t inTotalThreads = 1);
  ~HttpServer();

  HttpServer(const HttpServer& other) = delete;
  HttpServer(HttpServer&& other) = delete;
  HttpServer& operator=(const HttpServer& other) = delete;
  HttpServer& operator=(HttpServer&& other) = delete;

public:
  void setOnConnectionCallback(
    const http_callbacks::OnConnection& onRequestCallback);

public:
  void start();
  void stop();

private:
  void _doAccept();
  void _onAccept(beast::error_code ec, boost::asio::ip::tcp::socket socket);

private:
  net::io_context _ioc;
  boost::asio::ip::tcp::acceptor _acceptor;
  boost::asio::ip::tcp::socket _socket;
  uint32_t _totalThreads;

private:
  http_callbacks::OnConnection _onConnectionCallback = nullptr;
  std::vector<std::thread> _allThreads;
};
