
#include "WebSocketConnection.hpp"


#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/strand.hpp>
#include <cstdlib>
#include <functional>
#include <memory>
#include <string>

#include <iostream>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

WebSocketConnection::SendBuffer::SendBuffer(const char* dataToSend, std::size_t dataSize) : size(dataSize) {
  if (dataSize > max_send_buffer_size) {
    throw std::runtime_error("send buffer requested size is too big");
  }

  std::memcpy(data, dataToSend, dataSize);
}

//
//
//
//
//

// Resolver and socket require an io_context
// explicit
WebSocketConnection::WebSocketConnection()
  : _ioc(1U)
  , _resolver(net::make_strand(_ioc))
  , _ws(net::make_strand(_ioc))
{
  _startThread();
}


WebSocketConnection::~WebSocketConnection() {
  disconnect();
  _stopThread();
}

AbstractWebSocketConnection&
WebSocketConnection::setOnOpenCallback(const OnOpenCallback& inOnOpenCallback) {
  _onOpenCallback = inOnOpenCallback;
  return *this;
}

AbstractWebSocketConnection&
WebSocketConnection::setOnErrorCallback(const OnErrorCallback& inOnErrorCallback) {
  _onErrorCallback = inOnErrorCallback;
  return *this;
}

AbstractWebSocketConnection&
WebSocketConnection::setOnCloseCallback(const OnCloseCallback& inOnCloseCallback) {
  _onCloseCallback = inOnCloseCallback;
  return *this;
}

AbstractWebSocketConnection&
WebSocketConnection::setOnMessageCallback(const OnMessageCallback& inOnMessageCallback) {
  _onMessageCallback = inOnMessageCallback;
  return *this;
}



// Start the asynchronous operation
void
WebSocketConnection::connect(
  std::string_view ipAddress,
  std::string_view port
) {
  disconnect();

  // Save these for later
  _ipAddress = ipAddress;
  _port = port;

  _workerThread->execute([this]()
  {
    this->_ioc.run();
  });

  // connect() -> on_resolve() -> on_connect()

  // Look up the domain name
  _resolver.async_resolve(
    _ipAddress,
    _port,
    beast::bind_front_handler(
      &WebSocketConnection::on_resolve,
      shared_from_this()));
}

void
WebSocketConnection::on_resolve(
  beast::error_code ec,
  tcp::resolver::results_type results)
{
  if (ec) {
    return _failed(ec, "resolve");
  }

  // Set the timeout for the operation
  beast::get_lowest_layer(_ws).expires_after(std::chrono::seconds(30));

  // Make the connection on the IP address we get from a lookup
  beast::get_lowest_layer(_ws).async_connect(
    results,
    beast::bind_front_handler(
      &WebSocketConnection::on_connect,
      shared_from_this()));
}

void
WebSocketConnection::on_connect(
  beast::error_code ec,
  tcp::resolver::results_type::endpoint_type
) {
  if (ec) {
    return _failed(ec, "connect");
  }

  // Turn off the timeout on the tcp_stream, because
  // the websocket stream has its own timeout system.
  beast::get_lowest_layer(_ws).expires_never();

  // Set suggested timeout settings for the websocket
  _ws.set_option(
    websocket::stream_base::timeout::suggested(
      beast::role_type::client));

  // Set a decorator to change the User-Agent of the handshake
  _ws.set_option(websocket::stream_base::decorator(
    [](websocket::request_type& req)
    {
      req.set(http::field::user_agent,
        std::string(BOOST_BEAST_VERSION_STRING) +
          " websocket-client-async");
    }));

  // Perform the websocket handshake
  _ws.async_handshake(_ipAddress, "/",
    beast::bind_front_handler(
      &WebSocketConnection::on_handshake,
      shared_from_this()));
}

void
WebSocketConnection::on_handshake(beast::error_code ec)
{
  if(ec) {
    return _failed(ec, "handshake");
  }

  _isConnected = true;

  _doRead();

  if (_onOpenCallback) {
    _onOpenCallback();
  }
}

void
WebSocketConnection::_doRead() {

  auto self = shared_from_this();

  // Read a message into our buffer
  _ws.async_read(
    _readBuffer,
    beast::bind_front_handler(
      &WebSocketConnection::on_read,
      shared_from_this())
    );
}

void
WebSocketConnection::_failed(beast::error_code ec, char const* what)
{
  std::cerr << "WebSocketConnection:failure \"" << what << "\": \"" << ec.message() << "\"" << std::endl;
  if (_onErrorCallback) {
    _onErrorCallback(ec.message());
  }

  // alternative disconnect
  _isConnected = false;

  _stop();
}

void
WebSocketConnection::_stop()
{
  if (!_isConnected) {
    return;
  }
  _isConnected = false;

  // the worker thread task will be completed
  // -> but it will remain running for any other new connection attempt
  _ioc.stop();
}

void
WebSocketConnection::on_read(
    beast::error_code ec,
    std::size_t bytes_transferred)
{
  static_cast<void>(bytes_transferred); // unused

  if (!_isConnected) {
    return;
  }

  if(ec) {
    _failed(ec, "read");
    return;
  }

  if (_onMessageCallback) {

    const auto& subBuffer = _readBuffer.data();
    const uint8_t* dataPtr = static_cast<const uint8_t*>(subBuffer.data());
    const uint32_t dataLength = uint32_t(subBuffer.size());

    _onMessageCallback(dataLength, dataPtr);
  }

  // Clear the buffer
  _readBuffer.consume(_readBuffer.size());

  _doRead();
}


bool WebSocketConnection::sendUtf8Text(const char* inText)
{
  return sendBinary(reinterpret_cast<const void*>(inText), std::strlen(inText));
}
bool WebSocketConnection::sendBinary(const void* inData, std::size_t inSize)
{
  if (!_isConnected) {
    return false;
  }

  ScopedAtomicLock lock(_atomicLock);

  const bool wasEmpty = _buffersToSend.empty();
  _buffersToSend.push_back(SendBuffer(reinterpret_cast<const char*>(inData), inSize));

  if (wasEmpty == true) {
    _doWrite();
  }
  return true;
}

bool
WebSocketConnection::isConnected() const
{
  return _isConnected;
}

void
WebSocketConnection::disconnect() {

  if (!_isConnected) {
    return;
  }

  _stop();

  _ws.close(websocket::close_code::normal);
}

void
WebSocketConnection::_onWrite(beast::error_code ec, std::size_t bytes_transferred) {
  boost::ignore_unused(bytes_transferred);

  ScopedAtomicLock lock(_atomicLock);

  if (!_isConnected) {
    _buffersToSend.clear();
    return;
  }

  if (ec) {
    _failed(ec, "write");
    return;
  }

  if (_buffersToSend.empty() == false) {

    // remove the buffer that got sent
    _buffersToSend.pop_front();

    // more to send?
    if (_buffersToSend.empty() == false) {
      _doWrite();
    }

  }
}

void
WebSocketConnection::_doWrite()
{
  const SendBuffer& buffer = _buffersToSend.front();

  _ws.async_write(
    boost::asio::buffer(buffer.data, buffer.size),
    beast::bind_front_handler(&WebSocketConnection::_onWrite, shared_from_this()));
}

// void
// on_close(beast::error_code ec)
// {
//   if(ec)
//     return fail(ec, "close");

//   // If we get here then the connection is closed gracefully

//   // The make_printable() function helps print a ConstBufferSequence
//   std::cout << beast::make_printable(_readBuffer.data()) << std::endl;
// }



void WebSocketConnection::_startThread()
{
  _workerThread = AbstractWorkerThread::create();
}

void WebSocketConnection::_stopThread()
{
  _workerThread->quit();
  _workerThread.reset(nullptr);
}



