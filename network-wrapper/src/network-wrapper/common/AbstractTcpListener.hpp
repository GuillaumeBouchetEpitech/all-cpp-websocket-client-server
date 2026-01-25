
#pragma once

#include "internals/boostHeaders.hpp"

#include <functional>
#include <memory>

using OnNewConnection = std::function<void(boost::asio::ip::tcp::socket&&)>;

class AbstractTcpListener {
public:
  AbstractTcpListener() = default;
  virtual ~AbstractTcpListener() = default;

  AbstractTcpListener(const AbstractTcpListener& other) = delete;
  AbstractTcpListener(AbstractTcpListener&& other) = delete;
  AbstractTcpListener& operator=(const AbstractTcpListener& other) = delete;
  AbstractTcpListener& operator=(AbstractTcpListener&& other) = delete;

public:
  static std::shared_ptr<AbstractTcpListener> create(
    net::io_context& ioc,
    boost::asio::ip::tcp::endpoint endpoint,
    bool useBoostStrands);

public:
  virtual void setOnNewConnectionCallback(const OnNewConnection& onNewConnectionCallback) = 0;
  virtual void start() = 0;
  virtual void stop() = 0;
};
