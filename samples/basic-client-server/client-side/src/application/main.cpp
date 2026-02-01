
#include "Application.hpp"
#include "defines.hpp"

#include "utilities/TraceLogger.hpp"

#include <cstdio>
#include <cstdlib> // <= EXIT_SUCCESS
#include <cstring> // memset, memcpy
#include <iostream>

#if defined D_NATIVE_BUILD

#include "network-wrapper/websocket-client/AbstractWebSocketConnection.hpp"

#include <thread>

int
main() {
  std::cout << "start" << std::endl;

  auto const host = "127.0.0.1";
  auto const port = "8888";

  Application _myApplication;
  _myApplication.connect(host, port);

  // native dumb "main loop"
  while (!_myApplication.isDone()) {
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }

  std::cout << "end" << std::endl;

  return EXIT_SUCCESS;
}

#else

#include "network-wrapper/websocket-client/wasm/createWebSocketConnection.hpp"

#include <emscripten/emscripten.h>
#include <emscripten/websocket.h>

namespace {

Application* _myApplication = nullptr;

}

extern "C" {

EMSCRIPTEN_KEEPALIVE
void
startApplication(const char* inHost, const char* inPort) {
  if (_myApplication) {
    return;
  }

  D_LOG_OUT("start");
  D_LOG_OUT(" -> inHost \"" << inHost << "\"");
  D_LOG_OUT(" -> inPort \"" << inPort << "\"");

  if (!emscripten_websocket_is_supported()) {
    D_LOG_ERR("websocket is not supported");
    return;
  }

  _myApplication = new Application();
  _myApplication->connect(inHost, inPort);
}

EMSCRIPTEN_KEEPALIVE
void
updateApplication(uint32_t inDelta) {
  if (!_myApplication)
    return;

  _myApplication->update(inDelta);
}

EMSCRIPTEN_KEEPALIVE
void
renderApplication() {
  if (!_myApplication)
    return;

  _myApplication->render();
}
}

#endif
