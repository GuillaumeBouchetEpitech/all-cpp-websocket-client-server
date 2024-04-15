
#pragma once

#include <cstdlib> // std::size_t

class IWebSocketSession {
public:
  void* userData = nullptr;

public:
  virtual ~IWebSocketSession() = default;

public:
  virtual void write(const char* data, std::size_t length) = 0;
};
