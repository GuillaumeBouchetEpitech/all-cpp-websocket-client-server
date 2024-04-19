
#pragma once

#if defined __EMSCRIPTEN__
#error this file should only be included for a native build
#endif

#include "../../AbstractWebSocketConnection.hpp"

#include "worker-thread/AbstractWorkerThread.hpp"

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/strand.hpp>
#include <cstdlib>
#include <functional>
#include <memory>
#include <string>
#include <thread>
#include <mutex>
#include <list>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>



constexpr std::size_t max_send_buffer_size = 255;

struct SendBuffer {
  std::size_t size = 0;
  char data[max_send_buffer_size];

  SendBuffer(const char* data, std::size_t size);
};


//------------------------------------------------------------------------------

// Sends a WebSocket message and prints the response
class WebSocketConnection : public AbstractWebSocketConnection, public std::enable_shared_from_this<WebSocketConnection>
{
public:
  // Resolver and socket require an io_context
  explicit
  WebSocketConnection();

  ~WebSocketConnection();

public:
  // Start the asynchronous operation
  void
  connect(
    std::string_view ipAddress,
    std::string_view port) override;

private:
  void
  on_resolve(
    beast::error_code ec,
    tcp::resolver::results_type results);

  void
  on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type);

  void
  on_handshake(beast::error_code ec);

  void
  on_read(
      beast::error_code ec,
      std::size_t bytes_transferred);

public:
  AbstractWebSocketConnection& setOnOpenCallback(const OnOpenCallback& inOnOpenCallback) override;
  AbstractWebSocketConnection& setOnErrorCallback(const OnErrorCallback& inOnErrorCallback) override;
  AbstractWebSocketConnection& setOnCloseCallback(const OnCloseCallback& inOnCloseCallback) override;
  AbstractWebSocketConnection& setOnMessageCallback(const OnMessageCallback& inOnMessageCallback) override;

public:
  void disconnect() override;

public:
  bool sendUtf8Text(const char* inText) override;
  bool sendBinary(const void* inData, std::size_t inSize) override;

public:
  bool isConnected() const override;

private:
  void _onWrite(beast::error_code ec, std::size_t bytes_transferred);
  void _doWrite();
  void _doRead();

  void _failed(beast::error_code ec, char const* what);
  void _stop();

  void _startThread();
  void _stopThread();

private:
  bool _isConnected = false;
  net::io_context _ioc;

  std::unique_ptr<AbstractWorkerThread> _workerThread;

  tcp::resolver _resolver;
  websocket::stream<beast::tcp_stream> _ws;
  beast::flat_buffer _readBuffer;
  std::string _ipAddress;
  std::string _port;
  std::list<SendBuffer> _buffersToSend;

private:
  OnOpenCallback _onOpenCallback = nullptr;
  OnErrorCallback _onErrorCallback = nullptr;
  OnCloseCallback _onCloseCallback = nullptr;
  OnMessageCallback _onMessageCallback = nullptr;

};

//------------------------------------------------------------------------------
