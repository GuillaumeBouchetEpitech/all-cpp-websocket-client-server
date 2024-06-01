
#pragma once

#include "network-wrapper/websocket-client/AbstractWebSocketConnection.hpp"

#include <string_view>
#include <memory>

class Application {

public:
  Application();
  ~Application() = default;

public:
  void connect(std::string_view inHost, std::string_view inPort);

public:
  void update(uint32_t deltaTime);
  void render();

public:
  bool isDone() const;

private:
  std::shared_ptr<AbstractWebSocketConnection> _webSocket = nullptr;
  bool _isDone = false;
};
