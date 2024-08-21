


#pragma once

#include <functional>
#include <string_view>
#include <memory>



class AbstractWebSocketConnection {
public:
  using OnOpenCallback = std::function<void()>;
  using OnErrorCallback = std::function<void(std::string_view)>;
  using OnCloseCallback = std::function<void(std::string_view)>;
  using OnMessageCallback = std::function<void(uint32_t sizeReceived, const uint8_t* dataReceived)>;

public:
  AbstractWebSocketConnection() = default;
  virtual ~AbstractWebSocketConnection() = default;

  AbstractWebSocketConnection(const AbstractWebSocketConnection& other) = delete;
  AbstractWebSocketConnection(AbstractWebSocketConnection&& other) = delete;

  AbstractWebSocketConnection& operator=(const AbstractWebSocketConnection& other) = delete;
  AbstractWebSocketConnection& operator=(AbstractWebSocketConnection&& other) = delete;

public:
  virtual AbstractWebSocketConnection& setOnOpenCallback(const OnOpenCallback& inOnOpenCallback) = 0;
  virtual AbstractWebSocketConnection& setOnErrorCallback(const OnErrorCallback& inOnErrorCallback) = 0;
  virtual AbstractWebSocketConnection& setOnCloseCallback(const OnCloseCallback& inOnCloseCallback) = 0;
  virtual AbstractWebSocketConnection& setOnMessageCallback(const OnMessageCallback& inOnMessageCallback) = 0;

public:
  virtual void connect(std::string_view inHost, std::string_view inPort) = 0;
  virtual void disconnect() = 0;

public:
  virtual bool sendUtf8Text(const char* inText) = 0;
  virtual bool sendBinary(const void* inData, std::size_t inSize) = 0;

public:
  [[nodiscard]] virtual bool isConnected() const = 0;

public:
  [[nodiscard]] static std::shared_ptr<AbstractWebSocketConnection> create();

};


