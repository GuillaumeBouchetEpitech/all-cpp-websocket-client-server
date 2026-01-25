
#pragma once

#if defined __EMSCRIPTEN__
#error this file should only be included for a native build
#endif

#include "../AbstractWebSocketConnection.hpp"

#include <memory>

[[nodiscard]] std::shared_ptr<AbstractWebSocketConnection> createWebSocketConnection();
