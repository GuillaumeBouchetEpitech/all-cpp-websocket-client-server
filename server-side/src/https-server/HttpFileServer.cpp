
#include "HttpFileServer.hpp"

#include <iostream>

HttpFileServer::HttpFileServer(
  const std::string& inBasePath,
  const std::string& inIpAddress,
  uint16_t inHttpPort,
  uint32_t inTotalThreads)
: _fileManager(inBasePath)
, _httpServer(net::ip::make_address(inIpAddress), inHttpPort, inTotalThreads)
{}

HttpFileServer::~HttpFileServer()
{
  _httpServer.stop();
}

void
HttpFileServer::start() {
  _httpServer.setOnConnectionCallback(
    [this](
      const http::request<http::dynamic_body>& request,
      http::response<http::dynamic_body>& response) {
      response.version(request.version());
      response.keep_alive(false);

      switch (request.method()) {
      case http::verb::get:
        _onGetRequest(request, response);
        break;

      default:
        // We return responses indicating an error if
        // we do not recognize the request method.
        response.result(http::status::bad_request);
        response.set(http::field::content_type, "text/plain");
        beast::ostream(response.body())
          << "Invalid request-method '" << std::string(request.method_string())
          << "'";
        break;
      }
    });

  _httpServer.start();
}

void
HttpFileServer::stop() {
  _httpServer.stop();
}


void HttpFileServer::setCustomHandler(const CustomHandler& handler)
{
  _customHandler = handler;
}


void
HttpFileServer::_onGetRequest(
  const http::request<http::dynamic_body>& request,
  http::response<http::dynamic_body>& response) {
  response.result(http::status::ok);
  response.set(http::field::server, "Beast");

  const std::string finalPath(request.target().begin(), request.target().end());

  if (_customHandler(finalPath, request, response))
  {
    // ...
  }
  else if (auto cache = _fileManager.getFile(finalPath)) {

    // cached file was found

    response.set(http::field::content_type, cache->type);
    response.set(http::field::last_modified, cache->lastModified);

    // cached file is compressed

    if (_isGzipCompressionPossible(*cache, request)) {

      // response payload will be compressed

      response.set(http::field::content_encoding, "gzip");

      beast::ostream(response.body()) << cache->compressed;

      const std::size_t payloadSize = response.payload_size()
                                        ? response.payload_size().get()
                                        : cache->fileContent.size();

      std::cout << "[HTTP] GET 200"
                << " [" << payloadSize << "b]"
                << " " << request.target() << " " << cache->type << " "
                << cache->compressionRatio << " " << cache->lastModified
                << std::endl;

    } else {

      // response payload will not be compressed

      beast::ostream(response.body()) << cache->fileContent;

      const std::size_t payloadSize = response.payload_size()
                                        ? response.payload_size().get()
                                        : cache->fileContent.size();
      std::cout << "[HTTP] GET 200"
                << " [" << payloadSize << "b]"
                << " " << request.target() << " " << cache->type << " "
                << cache->compressionRatio << " " << cache->lastModified
                << std::endl;
    }

  } else {

    // cached file was not found

    response.result(http::status::not_found);
    response.set(http::field::content_type, "text/plain");
    beast::ostream(response.body()) << "File not found\r\n";

    std::cout << "[HTTP] GET 404 " << request.target() << std::endl;
  }
}

bool
HttpFileServer::_isGzipCompressionPossible(
  const FileCacheResult& cache,
  const http::request<http::dynamic_body>& request) {

  // cache file compressed?
  if (cache.compressionRatio <= 0.0f)
    return false;

  // relevant request header present?
  const boost::beast::string_view encoding = request["accept-encoding"];
  if (encoding.empty())
    return false;

  // relevant request header value present?
  const std::size_t found = encoding.find("gzip");
  if (found == std::string::npos)
    return false;

  return true;
}
