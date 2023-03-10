
#include "https-server/HttpFileServer.hpp"
#include "websocket-server/WebSocketMainLogicServer.hpp"

#include <iostream>

int
main(int argc, char* argv[]) {
  // Check command line arguments.
  if (argc != 5) {
    std::cerr << "Usage: " << argv[0]
              << " <ip-address> <http-port> <ws-port> <threads>\n"
              << "Example:\n"
              << "    " << argv[0] << " 0.0.0.0 7777 8888 1\n";
    return EXIT_FAILURE;
  }

  const std::string ipAddress = argv[1];
  const uint16_t httpPort = static_cast<uint16_t>(std::atoi(argv[2]));
  const uint16_t wsPort = static_cast<uint16_t>(std::atoi(argv[3]));
  const uint32_t threads = std::max<uint32_t>(1, std::atoi(argv[4]));

  std::cout << "starting server" << std::endl;
  std::cout << "  ip-address: \"" << argv[1] << "\"" << std::endl;
  std::cout << "  http-port:  \"" << argv[2] << "\"" << std::endl;
  std::cout << "  ws-port:    \"" << argv[3] << "\"" << std::endl;
  std::cout << "  threads:    \"" << argv[4] << "\"" << std::endl;

  HttpFileServer httpFileServer("./files", ipAddress, httpPort, threads);
  WebSocketMainLogicServer webSocketServer(ipAddress, wsPort, threads);

  //
  // handle web socket config route
  //

  const std::string_view webSocketConfigPath = "/web-socket-config.json";
  const std::string webSocketConfigPayload = "{ \"webSocketUrl\": \"ws://" + ipAddress + ":" + std::to_string(wsPort) + "/\" }";

  auto customHandler = [&webSocketConfigPath, &webSocketConfigPayload](
    const std::string& path,
    const http::request<http::dynamic_body>& request,
    http::response<http::dynamic_body>& response)
  {
    static_cast<void>(request); // unused

    if (path == webSocketConfigPath)
    {
      response.set(http::field::content_type, "application/json");
      beast::ostream(response.body()) << webSocketConfigPayload;
      return true;
    }
    return false;
  };
  httpFileServer.setCustomHandler(customHandler);

  //
  // start http and websocket servers
  //

  httpFileServer.start();
  webSocketServer.start();

  //
  // print stuff...?
  //

  std::cout << "servers started" << std::endl;
  std::cout << " -> link: \"http://" << ipAddress << ":" << httpPort << "/\"" << std::endl;

  //
  // loop forever...
  // that could be where a shell would be setup...
  // ...or anything else allowing any kind of user input(s) (admin commands, etc.)
  //

  while (true)
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  return EXIT_SUCCESS;
}