
#pragma once

#include "network-wrapper/http-server/AbstractHttpServer.hpp"
#include "network-wrapper/http-server/callbacks.hpp"

#include "file-manager/FileManager.hpp"

class HttpFileServer {

public:
  using CustomHandler = std::function<bool(
    const std::string&, const http_callbacks::request&,
    http_callbacks::response&)>;

public:
  HttpFileServer(
    const std::string& basePath, const std::string& ipAddress,
    uint16_t httpPort, uint32_t totalThreads);

  ~HttpFileServer();

public:
  void setCustomHandler(const CustomHandler& handler);

public:
  void start();
  void stop();

private:
  void _onGetRequest(
    const http_callbacks::request& request, http_callbacks::response& response);

  static bool _isGzipCompressionPossible(
    const FileCacheEntry& cache, const http_callbacks::request& request);

private:
  FileManager _fileManager;
  std::shared_ptr<AbstractHttpServer> _httpServer;

  CustomHandler _customHandler;
};
