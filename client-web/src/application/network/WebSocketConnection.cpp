
#include "WebSocketConnection.hpp"

#include <iostream>

WebSocketConnection::~WebSocketConnection() { disconnect(); }

WebSocketConnection&
WebSocketConnection::setOnOpenCallback(const OnOpenCallback& inOnOpenCallback) {
  _onOpenCallback = inOnOpenCallback;
  return *this;
}

WebSocketConnection&
WebSocketConnection::setOnErrorCallback(
  const OnErrorCallback& inOnErrorCallback) {
  _onErrorCallback = inOnErrorCallback;
  return *this;
}

WebSocketConnection&
WebSocketConnection::setOnCloseCallback(
  const OnCloseCallback& inOnCloseCallback) {
  _onCloseCallback = inOnCloseCallback;
  return *this;
}

WebSocketConnection&
WebSocketConnection::setOnMessageCallback(
  const OnMessageCallback& inOnMessageCallback) {
  _onMessageCallback = inOnMessageCallback;
  return *this;
}

void
WebSocketConnection::connect(const char* inUrl) {
  if (_isConnected) {
    disconnect();
  }

  // typedef struct EmscriptenWebSocketCreateAttributes {
  //   // The target URL to connect to. This string can point to a stack local
  //   variable, the string is read immediately at a call to
  //   emscripten_websocket_new(). const char *url;
  //   // A comma-separated list of protocol strings. Set to e.g.
  //   "binary,base64" to create a WebSocket connection with two supported
  //   protocols "binary" and "base64".
  //   // Be careful to avoid leading and trailing spaces, e.g. "binary, base64"
  //   may not be interpreted properly.
  //   // This string can point to a stack local variable, the string is read
  //   immediately at a call to emscripten_websocket_new(). const char
  //   *protocols;

  //   // If true, the created socket will reside on the main browser thread. If
  //   false, the created socket is bound to the calling thread.
  //   // If you want to share the created EMSCRIPTEN_WEBSOCKET_T structure
  //   across multiple threads, or are running your own main loop in the
  //   // pthread that you create the socket, set createOnMainThread to true. If
  //   the created WebSocket only needs to be accessible on the thread
  //   // that created it, and the creating thread is an event based thread
  //   (meaning it regularly yields back to the browser event loop), then
  //   // it is more efficient to set this to false.
  //   EM_BOOL createOnMainThread;
  // } EmscriptenWebSocketCreateAttributes;

  EmscriptenWebSocketCreateAttributes ws_attrs = {inUrl, NULL, EM_TRUE};

  _wsSocket = emscripten_websocket_new(&ws_attrs);
  emscripten_websocket_set_onopen_callback(_wsSocket, this, _emOnOpen);
  emscripten_websocket_set_onerror_callback(_wsSocket, this, _emOnError);
  emscripten_websocket_set_onclose_callback(_wsSocket, this, _emOnClose);
  emscripten_websocket_set_onmessage_callback(_wsSocket, this, _emOnMessage);
}

void
WebSocketConnection::disconnect() {
  if (!_isConnected) {
    return;
  }

  EMSCRIPTEN_RESULT result =
    emscripten_websocket_close(_wsSocket, 1000, "no reason");
  if (result) {
    std::cerr << "Failed to emscripten_websocket_close(): " << result
              << std::endl;
  }

  _isConnected = false;
}

bool
WebSocketConnection::sendUtf8Text(const char* inText) {
  if (!_isConnected) {
    return false;
  }

  EMSCRIPTEN_RESULT result =
    emscripten_websocket_send_utf8_text(_wsSocket, inText);
  if (result) {
    std::cerr << "Failed to send utf8 text payload, result: " << result
              << std::endl;
    return false;
  }

  return true;
}

bool
WebSocketConnection::sendBinary(void* inData, std::size_t inSize) {
  if (!_isConnected) {
    return false;
  }

  EMSCRIPTEN_RESULT result =
    emscripten_websocket_send_binary(_wsSocket, (void*)inData, inSize);
  if (result) {
    std::cerr << "Failed to send binary payload, result: " << result
              << std::endl;
    return false;
  }

  return true;
}

EM_BOOL
WebSocketConnection::_emOnOpen(
  int eventType, const EmscriptenWebSocketOpenEvent* websocketEvent,
  void* userData) {
  WebSocketConnection* self = static_cast<WebSocketConnection*>(userData);
  self->_onOpen(eventType, websocketEvent);
  return EM_TRUE;
}
EM_BOOL
WebSocketConnection::_emOnError(
  int eventType, const EmscriptenWebSocketErrorEvent* websocketEvent,
  void* userData) {
  WebSocketConnection* self = static_cast<WebSocketConnection*>(userData);
  self->_onError(eventType, websocketEvent);
  return EM_TRUE;
}
EM_BOOL
WebSocketConnection::_emOnClose(
  int eventType, const EmscriptenWebSocketCloseEvent* websocketEvent,
  void* userData) {
  WebSocketConnection* self = static_cast<WebSocketConnection*>(userData);
  self->_onClose(eventType, websocketEvent);
  return EM_TRUE;
}
EM_BOOL
WebSocketConnection::_emOnMessage(
  int eventType, const EmscriptenWebSocketMessageEvent* websocketEvent,
  void* userData) {
  WebSocketConnection* self = static_cast<WebSocketConnection*>(userData);
  self->_onMessage(eventType, websocketEvent);
  return EM_TRUE;
}

void
WebSocketConnection::_onOpen(
  int eventType, const EmscriptenWebSocketOpenEvent* websocketEvent) {

  _isConnected = true;

  if (_onOpenCallback) {
    _onOpenCallback(eventType, websocketEvent);
  }
}
void
WebSocketConnection::_onError(
  int eventType, const EmscriptenWebSocketErrorEvent* websocketEvent) {

  _isConnected = false;

  if (_onErrorCallback) {
    _onErrorCallback(eventType, websocketEvent);
  }
}
void
WebSocketConnection::_onClose(
  int eventType, const EmscriptenWebSocketCloseEvent* websocketEvent) {

  _isConnected = false;

  if (_onCloseCallback) {
    _onCloseCallback(eventType, websocketEvent);
  }
}
void
WebSocketConnection::_onMessage(
  int eventType, const EmscriptenWebSocketMessageEvent* websocketEvent) {

  if (_onMessageCallback) {
    _onMessageCallback(eventType, websocketEvent);
  }
}
