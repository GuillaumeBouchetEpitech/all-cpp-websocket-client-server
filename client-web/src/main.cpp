
#include "application/Application.hpp"
#include "application/defines.hpp"

#include "utilities/TraceLogger.hpp"

#include <cstdio>
#include <cstdlib> // <= EXIT_SUCCESS
#include <cstring> // memset, memcpy
#include <iostream>


#if defined D_NATIVE_BUILD

#include "./network-wrapper/native/createWebSocketConnection.hpp"

#include <thread>

// int main(int argc, char** argv)
int main()
{
  // // Check command line arguments.
  // if(argc != 3)
  // {
  //   std::cerr <<
  //     "Usage: websocket-client-async <host> <port>\n" <<
  //     "Example:\n" <<
  //     "    websocket-client-async echo.websocket.org 80\n";
  //   return EXIT_FAILURE;
  // }
  // auto const host = argv[1];
  // auto const port = argv[2];
  // // auto const text = argv[3];

  std::cout << "start" << std::endl;

  auto const host = "127.0.0.1";
  auto const port = "8888";
  // auto const host = "127.0.0.1:8888";

  // // Launch the asynchronous operation
  // auto webSocket = createWebSocketConnection();
  // webSocket->setOnOpenCallback([webSocket]()
  // {
  //   std::cout << "is open" << std::endl;
  //   webSocket->sendUtf8Text("lol");
  //   std::cout << "sent" << std::endl;
  // });
  // webSocket->setOnMessageCallback([](uint32_t sizeReceived, const uint8_t* dataReceived)
  // {
  //   std::string_view msg(reinterpret_cast<const char*>(dataReceived), sizeReceived);
  //   std::cout << "on message: " << msg << std::endl;
  // });
  // webSocket->setOnCloseCallback([](std::string_view reason)
  // {
  //   std::cout << "closed, reason: " << reason << std::endl;
  // });
  // webSocket->setOnErrorCallback([]()
  // {
  //   std::cout << "error" << std::endl;
  // });
  // webSocket->connect(host, port);

  auto webSocket = createWebSocketConnection();

  Application* _myApplication = nullptr;

  _myApplication = new Application(webSocket);
  _myApplication->connect(host, port);

  while (true) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }

  std::cout << "end" << std::endl;

  return EXIT_SUCCESS;
}

#else

#include "./network-wrapper/wasm/createWebSocketConnection.hpp"

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

  auto webSocket = createWebSocketConnection();

  _myApplication = new Application(webSocket);
  _myApplication->connect(inHost, inPort);
}
}

#endif
