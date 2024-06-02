

#include "network-wrapper/websocket-client/AbstractWebSocketConnection.hpp"
#include "network-wrapper/websocket-server/AbstractWebSocketServer.hpp"

#include <memory>
#include <mutex>
#include <vector>

#include <chrono> // std::chrono::milliseconds

#include <iomanip> // std::setprecision

#include "gtest/gtest.h"

namespace {

void
_runServer(uint32_t totalMsg = 10000U, const uint32_t totalThreads = 4, const bool useStrands = true) {

  const std::string& ipAddress = "127.0.0.1";
  const uint16_t port = 18000;
  const std::string portStr = std::to_string(port);

  std::mutex _serverMutex;
  std::vector<std::string> serverReceived;

  std::mutex _clientMutex;
  std::vector<std::string> clientReceived;

  std::shared_ptr<IWebSocketSession> _wsSession;

  auto _webSocketServer = AbstractWebSocketServer::create(ipAddress, port, totalThreads, useStrands);
  _webSocketServer->setOnConnectionCallback([&_wsSession](std::shared_ptr<IWebSocketSession> newWsSession) {
    // static_cast<void>(newWsSession); // unused

    _wsSession = newWsSession;

    std::cout << "[SERVER] new client" << std::endl;
  });

  _webSocketServer->setOnMessageCallback(
    [useStrands, &_serverMutex,
     &serverReceived](std::shared_ptr<IWebSocketSession> wsSession, const char* dataPtr, std::size_t dataLength) {
      // std::cout << "[SERVER] client messaged -> " << std::string_view(dataPtr, dataLength) << std::endl;

      std::string_view messageToSend = "pong!";

      if (useStrands) {
        serverReceived.push_back(std::string(dataPtr, dataLength));
        wsSession->write(messageToSend.data(), messageToSend.size());
      } else {
        std::unique_lock<std::mutex> lock(_serverMutex);
        serverReceived.push_back(std::string(dataPtr, dataLength));
        wsSession->write(messageToSend.data(), messageToSend.size());
      }
    });

  _webSocketServer->setOnDisconnectionCallback([](std::shared_ptr<IWebSocketSession> disconnectedWsSession) {
    std::cout << "[SERVER] client disconnected" << std::endl;

    static_cast<void>(disconnectedWsSession); // unused
  });

  //
  //
  //
  //
  //

  //
  //
  //
  //
  //

  //
  //
  //
  //
  //

  std::shared_ptr<AbstractWebSocketConnection> _webSocket = AbstractWebSocketConnection::create();

  _webSocket->setOnOpenCallback([&_webSocket]() {
    // std::cout << "[CLIENT] connected" << std::endl;
  });

  _webSocket->setOnErrorCallback([](std::string_view message) {
    std::cerr << "[CLIENT] error" << std::endl;
    std::cerr << "[CLIENT]   reason (size=" << message.size() << ") \"" << message << " \"" << std::endl;
  });

  _webSocket->setOnCloseCallback([](std::string_view reason) {
    std::cout << "[CLIENT] disconnected" << std::endl;
    std::cout << "[CLIENT]   reason (size=" << reason.size() << ") \"" << reason << " \"" << std::endl;
  });

  _webSocket->setOnMessageCallback(
    [useStrands, &_clientMutex, &clientReceived](const uint32_t sizeReceived, const uint8_t* dataReceived) {
      // std::cout << "[CLIENT] new message -> " << std::string_view(reinterpret_cast<const char*>(dataReceived),
      // sizeReceived) << std::endl;

      if (useStrands) {
        clientReceived.push_back(std::string(reinterpret_cast<const char*>(dataReceived), sizeReceived));
      } else {
        std::unique_lock<std::mutex> lock(_clientMutex);
        clientReceived.push_back(std::string(reinterpret_cast<const char*>(dataReceived), sizeReceived));
      }
    });

  //
  //
  //
  //
  //

  //
  //
  //
  //
  //

  //
  //
  //
  //
  //

  _webSocketServer->start();

  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  ASSERT_EQ(_webSocket->isConnected(), false);

  _webSocket->connect(ipAddress, portStr);

  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  ASSERT_EQ(_webSocket->isConnected(), true);

  serverReceived.reserve(totalMsg); // pre-allocate
  clientReceived.reserve(totalMsg); // pre-allocate

  for (uint32_t ii = 0; ii < totalMsg; ++ii) {
    _webSocket->sendUtf8Text("ping!");
  }

  for (int32_t ii = 0; ii < 10; ++ii) {

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    std::cout << " - serverReceived.size() " << serverReceived.size() << " / " << totalMsg << std::endl;
    std::cout << " - clientReceived.size() " << clientReceived.size() << " / " << totalMsg << std::endl;

    if (serverReceived.size() == totalMsg && clientReceived.size() == totalMsg) {
      break;
    }
  }

  std::cout << "serverReceived.size() " << serverReceived.size() << " / " << totalMsg << std::endl;
  std::cout << "clientReceived.size() " << clientReceived.size() << " / " << totalMsg << std::endl;

  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  std::cout << "CHECK" << std::endl;

  ASSERT_EQ(serverReceived.size(), totalMsg);
  for (uint32_t ii = 0; ii < totalMsg; ++ii) {
    ASSERT_EQ(serverReceived[ii], "ping!");
  }

  ASSERT_EQ(clientReceived.size(), totalMsg);
  for (uint32_t ii = 0; ii < totalMsg; ++ii) {
    ASSERT_EQ(clientReceived[ii], "pong!");
  }

  std::cerr << "webSocket->disconnect()" << std::endl;
  _webSocket->disconnect();
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  std::cerr << " -> DONE" << std::endl;

  std::cerr << "wsSession->disconnect()" << std::endl;
  _wsSession->disconnect();
  _wsSession = nullptr; // the session must be destroyed before its server
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  std::cerr << " -> DONE" << std::endl;

  std::cerr << "webSocketServer->stop()" << std::endl;
  _webSocketServer->stop();
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  std::cerr << " -> DONE" << std::endl;

  ASSERT_EQ(true, true);
}

} // namespace

TEST(web_socket_session, simple_session_10000_ping_pong_messages_1_thread) {

  const uint32_t totalMsg = 10000;
  const uint32_t totalThreads = 1;
  const bool useStrands = true; // will be ignored internally

  _runServer(totalMsg, totalThreads, useStrands);
}

TEST(web_socket_session, simple_session_10000_ping_pong_messages_4_thread_strand_used) {

  const uint32_t totalMsg = 10000;
  const uint32_t totalThreads = 4;
  const bool useStrands = true;

  _runServer(totalMsg, totalThreads, useStrands);
}

TEST(web_socket_session, simple_session_10000_ping_pong_messages_4_thread_strand_not_used) {

  const uint32_t totalMsg = 10000;
  const uint32_t totalThreads = 4;
  const bool useStrands = false;

  _runServer(totalMsg, totalThreads, useStrands);
}
