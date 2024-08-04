
#include "WebSocketMainLogicServer.hpp"

#include "network-wrapper/utilities/stdVectorNoReallocErase.hpp"
#include "network-wrapper/websocket-server/IWebSocketSession.hpp"

#include <iostream>
#include <sstream>

WebSocketMainLogicServer::WebSocketMainLogicServer(
  const std::string& ipAddress, uint16_t port, uint32_t totalThreads, bool useStrands)
  : _sessionManager(totalThreads > 1 && useStrands == false) // is locking?
{
  _allPlayersData.reserve(1024);

  _webSocketServer = AbstractWebSocketServer::create(ipAddress, port, totalThreads, useStrands);

  // feels easier to follow than directly using lambdas
  auto onNewClient = std::bind(&WebSocketMainLogicServer::_onNewClient, this, std::placeholders::_1);
  auto onNewMessage = std::bind(&WebSocketMainLogicServer::_onNewMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
  auto onLostClient = std::bind(&WebSocketMainLogicServer::_onLostClient, this, std::placeholders::_1);

  _webSocketServer->setOnConnectionCallback(onNewClient);
  _webSocketServer->setOnMessageCallback(onNewMessage);
  _webSocketServer->setOnDisconnectionCallback(onLostClient);
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

//
//
//

void WebSocketMainLogicServer::_onNewClient(std::shared_ptr<IWebSocketSession> newWsSession)
{
  std::cout << "[WS] new client" << std::endl;

  //
  //
  // create the new user data saved with the session

  auto newPlayerData = std::make_shared<PlayerData>();
  newPlayerData->id = _lastPlayerId++;
  newWsSession->userData = newPlayerData.get();

  _allPlayersData.push_back(newPlayerData);

  //
  //
  // broadcast new session to all existing session(s)

  {
    std::stringstream sstr;
    sstr << "[BROADCAST][NEW CLIENT]: " << newPlayerData->id;
    const std::string messageToSend = sstr.str();

    _sessionManager.forEachSession([&messageToSend](std::shared_ptr<IWebSocketSession> currWsSession) {
      currWsSession->write(messageToSend.data(), messageToSend.size());
    });
  }

  //
  //
  // add session

  _sessionManager.addSession(newWsSession);

  {
    std::stringstream sstr;
    sstr << "[YOU ONLY][WELCOME] your ID is \"" << newPlayerData->id << "\"";
    const std::string messageToSend = sstr.str();

    newWsSession->write(messageToSend.data(), messageToSend.size());
  }
}

//
//
//

void WebSocketMainLogicServer::_onNewMessage(std::shared_ptr<IWebSocketSession> wsSession, const char* dataPtr, std::size_t dataLength)
{
  std::string_view messageReceived(dataPtr, dataLength);

  const PlayerData* currPlayerData = static_cast<PlayerData*>(wsSession->userData);
  const int32_t playerId = currPlayerData ? currPlayerData->id : -1;

  //
  //
  // broadcast

  {
    std::stringstream sstr;
    sstr << "[BROADCAST][NEW MESSAGE][FROM \"" << playerId << "\"] \"" << messageReceived << "\"";

    const std::string messageToSend = sstr.str();

    std::cout << "[WS] " << messageToSend << std::endl;

    _sessionManager.forEachSession([&messageToSend, wsSession](std::shared_ptr<IWebSocketSession> currWsSession) {
      if (wsSession != currWsSession) {
        currWsSession->write(messageToSend.data(), messageToSend.size());
      }
    });
  }

  //
  //
  // feedback to the sender

  {
    std::stringstream sstr;
    sstr << "[BROADCAST][NEW MESSAGE][FROM YOU] \"" << messageReceived << "\"";

    const std::string messageToSend = sstr.str();

    wsSession->write(messageToSend.data(), messageToSend.size());
  }
};

//
//
//

void WebSocketMainLogicServer::_onLostClient(std::shared_ptr<IWebSocketSession> disconnectedWsSession)
{
  std::cout << "[WS] client disconnected" << std::endl;

  PlayerData* currPlayerData = static_cast<PlayerData*>(disconnectedWsSession->userData);
  const int32_t playerId = currPlayerData ? currPlayerData->id : -1;

  //
  //
  // remove session

  _sessionManager.removeSession(disconnectedWsSession);

  //
  //
  // remove data of the disconnected player

  stdVectorNoReallocEraseByCallback<std::shared_ptr<PlayerData>>(
    _allPlayersData, [currPlayerData](const std::shared_ptr<PlayerData>& currData) -> bool {
      return currData.get() == currPlayerData;
    });

  //
  //
  // broadcast

  std::stringstream sstr;
  sstr << "[BROADCAST][CLIENT LEFT][WAS \"" << playerId << "\"]";
  const std::string messageToSend = sstr.str();

  _sessionManager.forEachSession([&messageToSend](std::shared_ptr<IWebSocketSession> currWsSession) {
    currWsSession->write(messageToSend.data(), messageToSend.size());
  });
};
