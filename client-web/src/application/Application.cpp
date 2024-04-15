
#include "Application.hpp"

#include "utilities/TraceLogger.hpp"

Application::Application(std::shared_ptr<AbstractWebSocketConnection> webSocket)
  : _webSocket(webSocket)
{

}

void
Application::connect(std::string_view inHost, std::string_view inPort) {
  if (_webSocket) {
    _webSocket->disconnect();
  }

  _webSocket->setOnOpenCallback([this]() {

    D_LOG_OUT("> connected");
    D_LOG_OUT("> sending utf8 message: \"ping!\"");

    _webSocket->sendUtf8Text("ping!");
  });

  _webSocket->setOnErrorCallback([this](std::string_view message) {
    D_LOG_OUT("> error");
    D_LOG_OUT(">>> message (size=" << message.size() << ") \"" << message << " \"");

    _isDone = true;
  });

  _webSocket->setOnCloseCallback([this](std::string_view reason) {
    D_LOG_OUT("> disconnected");
    D_LOG_OUT(">>> reason (size=" << reason.size() << ") \"" << reason << " \"");

    _isDone = true;
  });

  _webSocket->setOnMessageCallback([](const uint32_t sizeReceived, const uint8_t* dataReceived) {

    auto tmpBuffer = std::make_unique<char[]>(sizeReceived + 1);

    std::memset(tmpBuffer.get(), 0, sizeReceived + 1);
    std::memcpy(tmpBuffer.get(), dataReceived, sizeReceived);

    const std::string_view message(tmpBuffer.get(), sizeReceived);

    D_LOG_OUT("> new msg (binary, size=" << message.size() << ") \"" << message << "\"");
  });

  _webSocket->connect(inHost, inPort);
}

bool Application::isDone() const
{
  return _isDone;
}
