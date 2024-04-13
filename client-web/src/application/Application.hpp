
#pragma once

#include "../network-wrapper/AbstractWebSocketConnection.hpp"

#include <string_view>
#include <memory>

class Application {

public:
  Application(std::shared_ptr<AbstractWebSocketConnection> webSocket);
  ~Application() = default;

public:
  void connect(std::string_view inHost, std::string_view inPort);

private:
  std::shared_ptr<AbstractWebSocketConnection> _webSocket = nullptr;
};
