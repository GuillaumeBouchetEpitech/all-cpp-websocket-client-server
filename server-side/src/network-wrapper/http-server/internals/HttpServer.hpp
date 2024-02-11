
#pragma once

#include "../AbstractHttpServer.hpp"

#include "../callbacks.hpp"

#include "HttpConnection.hpp"
#include "boostHeaders.hpp"

#include <vector>

class HttpServer : public AbstractHttpServer,
                   public std::enable_shared_from_this<HttpServer> {

public:
  HttpServer(
    const std::string& ipAddress, const uint16_t port,
    const uint32_t totalThreads = 1);
  ~HttpServer();

  HttpServer(const HttpServer& other) = delete;
  HttpServer(HttpServer&& other) = delete;
  HttpServer& operator=(const HttpServer& other) = delete;
  HttpServer& operator=(HttpServer&& other) = delete;

public:
  void setOnConnectionCallback(
    const http_callbacks::OnConnection& onRequestCallback) override;

public:
  void start() override;
  void stop() override;

private:
  void _doAccept();

private:
  net::io_context _ioc;
  boost::asio::ip::tcp::acceptor _acceptor;
  boost::asio::ip::tcp::socket _tcpSocket;
  uint32_t _totalThreads;

private:
  http_callbacks::OnConnection _onConnectionCallback = nullptr;
  std::vector<std::thread> _allThreads;
};
