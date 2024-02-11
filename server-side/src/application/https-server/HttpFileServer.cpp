
#include "HttpFileServer.hpp"

#include <boost/beast/core/ostream.hpp>
#include <boost/beast/http/status.hpp>
#include <boost/beast/http/verb.hpp>

#include <iomanip>
#include <iostream>

HttpFileServer::HttpFileServer(
  const std::string& basePath, const std::string& ipAddress,
  uint16_t httpPort, uint32_t totalThreads)
  : _fileManager(basePath) {
  _httpServer =
    AbstractHttpServer::create(ipAddress, httpPort, totalThreads);
}

HttpFileServer::~HttpFileServer() { _httpServer->stop(); }

void
HttpFileServer::start() {
  _httpServer->setOnConnectionCallback([this](
                                         const http_callbacks::request& request,
                                         http_callbacks::response& response) {
    response.version(request.version());
    response.keep_alive(false);

    switch (request.method()) {
    case boost::beast::http::verb::get: {
      _onGetRequest(request, response);
      break;
    }

    default: {
      // We return responses indicating an error if
      // we do not recognize the request method.
      response.result(boost::beast::http::status::bad_request);
      response.set(boost::beast::http::field::content_type, "text/plain");
      boost::beast::ostream(response.body())
        << "Invalid request-method '" << std::string(request.method_string())
        << "'";
      break;
    }
    }
  });

  _httpServer->start();
}

void
HttpFileServer::stop() {
  _httpServer->stop();
}

void
HttpFileServer::setCustomHandler(const CustomHandler& handler) {
  _customHandler = handler;
}

void
HttpFileServer::_onGetRequest(
  const http_callbacks::request& request, http_callbacks::response& response) {

  const std::string finalPath(request.target().begin(), request.target().end());

  if (_customHandler(finalPath, request, response)) {
    return;
  }

  auto output = boost::beast::ostream(response.body());

  auto result = _fileManager.getFile(finalPath);
  if (result.has_value() == false) {
    // cached file was not found

    response.result(boost::beast::http::status::not_found);
    response.set(boost::beast::http::field::server, "Beast");
    response.set(boost::beast::http::field::content_type, "text/plain");
    output << "File not found\r\n";

    std::cout << "[HTTP] GET 404 " << request.target() << std::endl;
    return;
  }

  const FileCacheEntry& cachedResult = *result;

  // cached file was found

  response.result(boost::beast::http::status::ok);
  response.set(boost::beast::http::field::server, "Beast");
  response.set(boost::beast::http::field::content_type, cachedResult.type);
  response.set(
    boost::beast::http::field::last_modified, cachedResult.lastModified);

  // cached file (compressed or not)

  if (_isGzipCompressionPossible(cachedResult, request)) {
    response.set(boost::beast::http::field::content_encoding, "gzip");
    output << cachedResult.compressedFileContent;
  } else {
    output << cachedResult.fileContent;
  }

  // simple log

  std::cout << "[HTTP] GET 200 " << cachedResult.logContent << std::endl;
}

bool
HttpFileServer::_isGzipCompressionPossible(
  const FileCacheEntry& cache, const http_callbacks::request& request) {

  // cache file compressed?
  if (cache.compressionRatio <= 0.0f) {
    return false;
  }

  // relevant request header present?
  const boost::beast::string_view encoding = request["accept-encoding"];
  if (encoding.empty()) {
    return false;
  }

  // relevant request header value present?
  const std::size_t foundIndex = encoding.find("gzip");
  if (foundIndex == std::string::npos) {
    return false;
  }

  return true;
}
