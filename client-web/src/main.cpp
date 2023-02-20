
#include "application/Application.hpp"

#include "utilities/TraceLogger.hpp"

#include <cstdio>
#include <cstdlib> // <= EXIT_SUCCESS
#include <cstring> // memset, memcpy
#include <iostream>

namespace {

Application* _myApplication = nullptr;

}

extern "C" {

EMSCRIPTEN_KEEPALIVE
void
startApplication(const char* inUrl) {
  if (_myApplication)
    return;

  D_LOG_OUT("start");
  D_LOG_OUT(" -> inUrl \"" << inUrl << "\"");

  if (!emscripten_websocket_is_supported()) {
    D_LOG_ERR("websocket is not supported");
    return;
  }

  _myApplication = new Application();
  _myApplication->connect(inUrl);
}
}
