
#pragma once

#include "network-wrappers/http-server/HttpServer.hpp"

#include "file-manager/FileManager.hpp"

class HttpFileServer {

public:
  using CustomHandler = std::function<bool(
    const std::string&,
    const http::request<http::dynamic_body>&,
    http::response<http::dynamic_body>&)>;

public:
  HttpFileServer(
    const std::string& inBasePath, const std::string& inIpAddress,
    uint16_t inHttpPort, uint32_t inTotalThreads);

  ~HttpFileServer();

public:
  void setCustomHandler(const CustomHandler& handler);

public:
  void start();
  void stop();

private:
  void _onGetRequest(
    const http::request<http::dynamic_body>& request,
    http::response<http::dynamic_body>& response);

  static bool _isGzipCompressionPossible(
    const FileCacheResult& cache,
    const http::request<http::dynamic_body>& request);

private:
  FileManager _fileManager;
  HttpServer _httpServer;

  CustomHandler _customHandler;

};
