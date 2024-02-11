
#include "WebSocketMainLogicServer.hpp"

#include "network-wrapper/websocket-server/IWebSocketSession.hpp"

#include <iostream>
#include <sstream>

WebSocketMainLogicServer::WebSocketMainLogicServer(
  const std::string& ipAddress, uint16_t port, uint32_t totalThreads) {
  _allPlayersData.reserve(1024);

  _webSocketServer =
    AbstractWebSocketServer::create(ipAddress, port, totalThreads);
  _webSocketServer->setOnConnectionCallback(
    [this](std::shared_ptr<IWebSocketSession> newWsSession) {
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
        [&messageToSend](std::shared_ptr<IWebSocketSession> currWsSession) {
          currWsSession->write(messageToSend.data(), messageToSend.size());
        });

      //
      //
      // add session

      _sessionManager.addSession(newWsSession);

      //
      //
      // add data

      _allPlayersData.push_back(newPlayerData);
      newWsSession->userData = newPlayerData.get();
    });

  _webSocketServer->setOnMessageCallback(
    [this](
      std::shared_ptr<IWebSocketSession> wsSession, const char* dataPtr,
      std::size_t dataLength) {
      std::string_view messageReceived(dataPtr, dataLength);

      const PlayerData* currPlayerData =
        static_cast<PlayerData*>(wsSession->userData);
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
           wsSession](std::shared_ptr<IWebSocketSession> currWsSession) {
            if (wsSession != currWsSession)
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

        wsSession->write(messageToSend.data(), messageToSend.size());
      }
    });

  _webSocketServer->setOnDisconnectionCallback(
    [this](std::shared_ptr<IWebSocketSession> disconnectedWsSession) {
      std::cout << "[WS] client disconnected" << std::endl;

      PlayerData* currPlayerData =
        static_cast<PlayerData*>(disconnectedWsSession->userData);
      const int32_t playerId = currPlayerData ? currPlayerData->id : -1;

      //
      //
      // remove session

      _sessionManager.removeSession(disconnectedWsSession);

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
        [&messageToSend](std::shared_ptr<IWebSocketSession> currWsSession) {
          currWsSession->write(messageToSend.data(), messageToSend.size());
        });
    });
}

WebSocketMainLogicServer::~WebSocketMainLogicServer() { stop(); }

void
WebSocketMainLogicServer::start() {
  _webSocketServer->start();
}

void
WebSocketMainLogicServer::stop() {
  _webSocketServer->stop();
}
