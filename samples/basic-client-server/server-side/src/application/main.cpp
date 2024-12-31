
#include "https-server/HttpFileServer.hpp"
#include "websocket-server/WebSocketMainLogicServer.hpp"

#include <boost/beast/core/ostream.hpp>

#include <iostream>

namespace {

std::string
_buildWebSocketUrl(const std::string_view ipAddress, uint16_t port) {
  std::stringstream sstr;
  sstr << "{";
  sstr << " \"host\": \"" << ipAddress << "\",";
  sstr << " \"port\": \"" << std::to_string(port) << "\"";
  sstr << " }";
  return sstr.str();
}

} // namespace

int
main(int argc, char* argv[]) {
  // Check command line arguments.
  if (argc != 6) {
    std::cerr << "Usage: " << argv[0] << " <ip-address> <http-port> <ws-port> <http-threads> <ws-threads>\n"
              << "Example:\n"
              << "    " << argv[0] << " 0.0.0.0 7777 8888 1 1\n";
    return EXIT_FAILURE;
  }

  const std::string ipAddress = argv[1];
  const uint16_t httpPort = static_cast<uint16_t>(std::atoi(argv[2]));
  const uint16_t wsPort = static_cast<uint16_t>(std::atoi(argv[3]));
  const uint32_t httpThreads = std::max<uint32_t>(1, std::atoi(argv[4]));
  const uint32_t wsThreads = std::max<uint32_t>(1, std::atoi(argv[5]));

  std::cout << "starting server" << std::endl;
  std::cout << "  ip-address:   \"" << argv[1] << "\"" << std::endl;
  std::cout << "  http-port:    \"" << argv[2] << "\"" << std::endl;
  std::cout << "  ws-port:      \"" << argv[3] << "\"" << std::endl;
  std::cout << "  http-threads: \"" << argv[4] << "\"" << std::endl;
  std::cout << "  ws-threads:   \"" << argv[5] << "\"" << std::endl;

  constexpr bool useStrands = true;

  HttpFileServer httpFileServer("./files", ipAddress, httpPort, httpThreads);
  WebSocketMainLogicServer webSocketServer(ipAddress, wsPort, wsThreads, useStrands);

  //
  // handle web socket config route
  //

  const std::string_view webSocketConfigPath = "/web-socket-config.json";
  const std::string webSocketConfigPayload = _buildWebSocketUrl(ipAddress, wsPort);

  auto customHandler =
    [&webSocketConfigPath, &webSocketConfigPayload](
      const std::string& path, const http_callbacks::request& request, http_callbacks::response& response) {
      static_cast<void>(request); // unused

      if (path == webSocketConfigPath) {
        response.set(boost::beast::http::field::content_type, "application/json");
        boost::beast::ostream(response.body()) << webSocketConfigPayload;
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
  // ...or anything else allowing any kind of user input(s)
  // ex:
  // -> admin commands
  // -> game loop
  // -> etc.
  //

  while (true) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }

  return EXIT_SUCCESS;
}