
#pragma once

#if defined __EMSCRIPTEN__
#error this file should only be included for a native build
#endif

#include "../AbstractHttpServer.hpp"

#include "../callbacks.hpp"

#include "../../common/AbstractTcpListener.hpp"
#include "HttpConnection.hpp"
#include "boostHeaders.hpp"

#include <vector>

class HttpServer
  : public AbstractHttpServer
  , public std::enable_shared_from_this<HttpServer> {

public:
  HttpServer(const std::string& ipAddress, const uint16_t port, const uint32_t totalThreads = 1);
  ~HttpServer();

  HttpServer(const HttpServer& other) = delete;
  HttpServer(HttpServer&& other) = delete;
  HttpServer& operator=(const HttpServer& other) = delete;
  HttpServer& operator=(HttpServer&& other) = delete;

public:
  void setOnConnectionCallback(const http_callbacks::OnConnection& onRequestCallback) override;

public:
  void start() override;
  void stop() override;

private:
  void _doAccept();

private:
  net::io_context _ioc;
  std::shared_ptr<AbstractTcpListener> _mainTcpListener;
  uint32_t _totalThreads;

private:
  http_callbacks::OnConnection _onConnectionCallback = nullptr;
  std::vector<std::thread> _allThreads;
};
