
#include "Application.hpp"

#include "utilities/TraceLogger.hpp"

void
Application::connect(const char* inUrl) {
  if (_webSocket) {
    _webSocket->disconnect();
  }

  _webSocket = std::make_unique<WebSocketConnection>();

  _webSocket->setOnOpenCallback([this](int eventType, const EmscriptenWebSocketOpenEvent* websocketEvent) {
    static_cast<void>(eventType);      // unused
    static_cast<void>(websocketEvent); // unused

    D_LOG_OUT("> connected");
    D_LOG_OUT("> sending utf8 message: \"ping!\"");

    _webSocket->sendUtf8Text("ping!");
  });

  _webSocket->setOnErrorCallback([](int eventType, const EmscriptenWebSocketErrorEvent* websocketEvent) {
    static_cast<void>(eventType);      // unused
    static_cast<void>(websocketEvent); // unused

    D_LOG_OUT("> error");
  });

  _webSocket->setOnCloseCallback([](int eventType, const EmscriptenWebSocketCloseEvent* websocketEvent) {
    static_cast<void>(eventType); // unused

    const std::string_view message(websocketEvent->reason);

    D_LOG_OUT("> disconnected");
    D_LOG_OUT(">>> reason (size=" << message.size() << ") \"" << message << " \"");
  });

  _webSocket->setOnMessageCallback([](int eventType, const EmscriptenWebSocketMessageEvent* websocketEvent) {
    static_cast<void>(eventType); // unused

    const uint32_t sizeReceived = websocketEvent->numBytes;
    const char* dataReceived = reinterpret_cast<const char*>(websocketEvent->data);

    if (websocketEvent->isText) {
      // For only ascii chars.

      const std::string_view message(dataReceived, sizeReceived);

      // D_LOG_OUT(" => message received (utf8 text)");
      D_LOG_OUT("> new msg (utf8, size=" << message.size() << ") \"" << message << "\"");
    } else {

      auto tmpBuffer = std::make_unique<char[]>(sizeReceived + 1);

      std::memset(tmpBuffer.get(), 0, sizeReceived + 1);
      std::memcpy(tmpBuffer.get(), dataReceived, sizeReceived);

      const std::string_view message(tmpBuffer.get(), sizeReceived);

      // D_LOG_OUT(" => message received (binary)");
      D_LOG_OUT("> new msg (binary, size=" << message.size() << ") \"" << message << "\"");
    }
  });

  _webSocket->connect(inUrl);
}
