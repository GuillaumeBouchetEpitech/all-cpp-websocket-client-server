
#pragma once

#include "network/WebSocketConnection.hpp"

#include <memory>

class Application {

public:
  Application() = default;
  ~Application() = default;

public:
  void connect(const char* inUrl);

private:
  std::unique_ptr<WebSocketConnection> _webSocket = nullptr;
};
