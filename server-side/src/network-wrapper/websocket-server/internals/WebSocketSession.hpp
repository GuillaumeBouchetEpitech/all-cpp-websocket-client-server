
#pragma once

#include "../IWebSocketSession.hpp"

#include "boostHeaders.hpp"

#include <list>
#include <memory>

class TcpListener;

constexpr std::size_t max_send_buffer_size = 255;

struct SendBuffer {
  std::size_t size = 0;
  char data[max_send_buffer_size];

  SendBuffer(const char* inData, std::size_t inSize);
};

// Echoes back all received WebSocket messages
class WebSocketSession : public IWebSocketSession,
                         public std::enable_shared_from_this<WebSocketSession> {

public:
  void* userData = nullptr;

public:
  // Take ownership of the socket
  explicit WebSocketSession(
    boost::asio::ip::tcp::socket&& inSocket, TcpListener& inMainTcpListener);

  // Get on the correct executor
  void run() override;

  void write(const char* data, std::size_t length) override;

private:
  // Start the asynchronous operation
  void _onRun();
  void _onAccept(beast::error_code ec);
  void _doRead();
  void _onRead(beast::error_code ec, std::size_t bytes_transferred);
  void _onWrite(beast::error_code ec, std::size_t bytes_transferred);

private:
  websocket::stream<beast::tcp_stream> _ws;
  beast::flat_buffer _buffer;
  TcpListener& _mainTcpListener;

  std::list<SendBuffer> _buffersToSend;
};
