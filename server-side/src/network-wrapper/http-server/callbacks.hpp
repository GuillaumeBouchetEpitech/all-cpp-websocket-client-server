
#include <boost/beast/http/dynamic_body.hpp> // http::dynamic_body
#include <boost/beast/http/message.hpp>      // http::request, http::response

#include <functional>

namespace http_callbacks {

namespace http = boost::beast::http;

using request = http::request<http::dynamic_body>;
using response = http::response<http::dynamic_body>;

using OnConnection = std::function<void(const request&, response&)>;

} // namespace http_callbacks
