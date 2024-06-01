
#pragma once

#if defined __EMSCRIPTEN__
#error this file should only be included for a native build
#endif

#include "../../utilities/TestAndSetAtomicLock.hpp"

#include "../IWebSocketSession.hpp"
#include "../callbacks.hpp"

#include "boostHeaders.hpp"

#include <list>
#include <memory>
#include <mutex>

constexpr std::size_t max_send_buffer_size = 255;

// Echoes back all received WebSocket messages
class WebSocketSession : public IWebSocketSession, public std::enable_shared_from_this<WebSocketSession> {

private:
  struct SendBuffer {
    std::size_t size = 0;
    char data[max_send_buffer_size];

    SendBuffer(const char* data, std::size_t size);
  };


public:
  void* userData = nullptr;

public:
  // Take ownership of the socket
  explicit WebSocketSession(
    boost::asio::ip::tcp::socket&& socket,
    bool useBoostStrands,
    std::shared_ptr<net::strand<net::any_io_executor>> strand,
    const ws_callbacks::OnConnection& onConnectionCallback,
    const ws_callbacks::OnDisconnection& onDisconnectionCallback,
    const ws_callbacks::OnMessage& onMessageCallback);

  WebSocketSession(const WebSocketSession& other) = delete;
  WebSocketSession& operator=(const WebSocketSession& other) = delete;
  WebSocketSession(WebSocketSession&& other) = delete;
  WebSocketSession& operator=(WebSocketSession&& other) = delete;

  ~WebSocketSession();

  // Get on the correct executor
  void run();
  void disconnect();

  void write(const char* data, std::size_t length) override;

private:
  // Start the asynchronous operation
  void _onRun();
  void _onAccept(beast::error_code ec);
  void _doRead();
  void _onRead(beast::error_code ec, std::size_t bytes_transferred);
  void _onWrite(beast::error_code ec, std::size_t bytes_transferred);
  void _doWrite();
  void _failed(beast::error_code ec, char const* what);

private:
  bool _isConnected = true;
  websocket::stream<beast::tcp_stream> _ws;
  std::shared_ptr<net::strand<net::any_io_executor>> _strand;
  bool _useBoostStrands;
  beast::flat_buffer _buffer;
  const ws_callbacks::OnConnection& _onConnectionCallback;
  const ws_callbacks::OnDisconnection& _onDisconnectionCallback;
  const ws_callbacks::OnMessage& _onMessageCallback;

  // std::mutex _writeMutex;
  TestAndSetAtomicLock _atomicLock;
  std::list<SendBuffer> _buffersToSend;
};
