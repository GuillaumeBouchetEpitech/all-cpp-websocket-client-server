
#include "IWebSocketSession.hpp"

#include <functional>
#include <memory>

namespace ws_callbacks {

using OnConnection = std::function<void(std::shared_ptr<IWebSocketSession>)>;
using OnDisconnection = OnConnection;
using OnMessage = std::function<void(
  std::shared_ptr<IWebSocketSession>, const char*, std::size_t)>;

} // namespace ws_callbacks
