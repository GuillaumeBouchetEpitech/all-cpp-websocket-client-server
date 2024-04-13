
#pragma once

#include "../AbstractWebSocketConnection.hpp"

#include <memory>

std::shared_ptr<AbstractWebSocketConnection>
createWebSocketConnection();
