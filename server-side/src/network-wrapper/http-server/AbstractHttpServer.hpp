
#pragma once

#include "callbacks.hpp"

class AbstractHttpServer {
public:
  AbstractHttpServer() = default;
  virtual ~AbstractHttpServer() = default;

  AbstractHttpServer(const AbstractHttpServer& other) = delete;
  AbstractHttpServer(AbstractHttpServer&& other) = delete;
  AbstractHttpServer& operator=(const AbstractHttpServer& other) = delete;
  AbstractHttpServer& operator=(AbstractHttpServer&& other) = delete;

public:
  virtual void setOnConnectionCallback(
    const http_callbacks::OnConnection& onRequestCallback) = 0;

public:
  virtual void start() = 0;
  virtual void stop() = 0;

public:
  static std::shared_ptr<AbstractHttpServer> create(
    const std::string& inIpAddress, uint16_t inPort,
    uint32_t inTotalThreads = 1);
};
