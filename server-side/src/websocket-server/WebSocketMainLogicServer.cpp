
#include "WebSocketMainLogicServer.hpp"

#include "network-wrappers/websocket-server/internal/WebSocketSession.hpp"

#include <iostream>

WebSocketMainLogicServer::WebSocketMainLogicServer(
  const std::string& inIpAddress, uint16_t inPort,
  uint32_t inTotalThreads)
  : _webSocketServer(net::ip::make_address(inIpAddress), inPort, inTotalThreads) {
  _allPlayersData.reserve(1024);

  _webSocketServer.setOnConnectionCallback(
    [this](std::shared_ptr<WebSocketSession> inNewWsSession) {
      std::cout << "[WS] new client" << std::endl;

      //
      //
      // broadcast

      auto newPlayerData = std::make_shared<PlayerData>();
      newPlayerData->id = _lastPlayerId++;

      std::stringstream sstr;
      sstr << "new client: " << newPlayerData->id;
      const std::string messageToSend = sstr.str();

      _sessionManager.forEachSession(
        [&messageToSend](std::shared_ptr<WebSocketSession> currWsSession) {
          currWsSession->write(messageToSend.data(), messageToSend.size());
        });

      //
      //
      // add session

      _sessionManager.addSession(inNewWsSession);

      //
      //
      // add data

      _allPlayersData.push_back(newPlayerData);
      inNewWsSession->userData = newPlayerData.get();
    });

  _webSocketServer.setOnMessageCallback(
    [this](
      std::shared_ptr<WebSocketSession> inWsSession,
      const beast::flat_buffer& buffer) {
      const auto& subBuffer = buffer.data();
      const char* dataPtr = static_cast<const char*>(subBuffer.data());

      std::string_view messageReceived(dataPtr, subBuffer.size());

      const PlayerData* currPlayerData =
        static_cast<PlayerData*>(inWsSession->userData);
      const int32_t playerId = currPlayerData ? currPlayerData->id : -1;

      //
      //
      // broadcast

      {
        std::stringstream sstr;
        sstr << "client (" << playerId << ") sent: \"" << messageReceived
             << "\"";
        const std::string messageToSend = sstr.str();

        std::cout << "[WS] " << messageToSend << std::endl;

        _sessionManager.forEachSession(
          [&messageToSend,
           inWsSession](std::shared_ptr<WebSocketSession> currWsSession) {
            if (inWsSession != currWsSession)
              currWsSession->write(messageToSend.data(), messageToSend.size());
          });
      }

      //
      //
      // feedback to the sender

      {
        std::stringstream sstr;
        sstr << "you (" << playerId << ") sent -> \"" << messageReceived
             << "\"";
        const std::string messageToSend = sstr.str();

        inWsSession->write(messageToSend.data(), messageToSend.size());
      }
    });

  _webSocketServer.setOnDisconnectionCallback(
    [this](std::shared_ptr<WebSocketSession> inDisconnectedWsSession) {
      std::cout << "[WS] client disconnected" << std::endl;

      PlayerData* currPlayerData =
        static_cast<PlayerData*>(inDisconnectedWsSession->userData);
      const int32_t playerId = currPlayerData ? currPlayerData->id : -1;

      //
      //
      // remove session

      _sessionManager.removeSession(inDisconnectedWsSession);

      //
      //
      // remove data of the disconnected player

      auto it = std::find_if(
        _allPlayersData.begin(), _allPlayersData.end(),
        [currPlayerData](const std::shared_ptr<PlayerData> currData) {
          return currData.get() == currPlayerData;
        });

      if (it != _allPlayersData.end()) {
        _allPlayersData.erase(it);
      }

      //
      //
      // broadcast

      std::stringstream sstr;
      sstr << "client disconnected: " << playerId;
      const std::string messageToSend = sstr.str();

      _sessionManager.forEachSession(
        [&messageToSend](std::shared_ptr<WebSocketSession> currWsSession) {
          currWsSession->write(messageToSend.data(), messageToSend.size());
        });
    });
}

WebSocketMainLogicServer::~WebSocketMainLogicServer() { stop(); }

void
WebSocketMainLogicServer::start() {
  _webSocketServer.start();
}

void
WebSocketMainLogicServer::stop() {
  _webSocketServer.stop();
}
