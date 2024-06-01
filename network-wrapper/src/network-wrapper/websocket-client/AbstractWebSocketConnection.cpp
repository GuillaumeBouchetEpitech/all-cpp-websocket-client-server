


#include "AbstractWebSocketConnection.hpp"



#if defined __EMSCRIPTEN__
#  include "wasm/createWebSocketConnection.hpp"
#else
#  include "native/createWebSocketConnection.hpp"
#endif



std::shared_ptr<AbstractWebSocketConnection>
AbstractWebSocketConnection::create() {
  return createWebSocketConnection();
}


