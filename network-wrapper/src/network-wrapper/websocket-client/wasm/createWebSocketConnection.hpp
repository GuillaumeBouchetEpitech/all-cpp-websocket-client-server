
#pragma once

#if not defined __EMSCRIPTEN__
#error this file should only be included for a wasm build
#endif

#include "../AbstractWebSocketConnection.hpp"

#include <memory>

std::shared_ptr<AbstractWebSocketConnection>
createWebSocketConnection();
