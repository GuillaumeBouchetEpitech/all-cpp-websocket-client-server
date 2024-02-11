

#include "TraceLogger.hpp"

#include <iomanip>
#include <iostream>
#include <memory>

#include <ctime>

namespace helpers {

std::string
TraceLogger::getTime() {
  std::time_t rawTime = std::time(0);
  std::tm* now = std::localtime(&rawTime);

  std::stringstream sstr;
  sstr << std::setfill('0') << std::setw(2) // <= left pad (size=2)
       << now->tm_hour << ":" << std::setfill('0')
       << std::setw(2) // <= left pad (size=2)
       << now->tm_min << ":" << std::setfill('0')
       << std::setw(2) // <= left pad (size=2)
       << now->tm_sec;

  return sstr.str();
}

void
TraceLogger::dumpOut() {
  auto lock = std::lock_guard<std::mutex>(_mutex);
  std::cout << _sstr.str() << std::endl;
}
void
TraceLogger::dumpErr() {
  auto lock = std::lock_guard<std::mutex>(_mutex);
  std::cerr << _sstr.str() << std::endl;
}

std::string
TraceLogger::getData() const {
  return _sstr.str();
}

template <>
TraceLogger&
TraceLogger::operator<< <bool>(bool data) {
  _sstr << std::boolalpha << data;
  return *this;
}

template <>
TraceLogger&
TraceLogger::operator<< <float>(float data) {
  _sstr << std::fixed << std::setprecision(2) << data;
  return *this;
}

template <>
TraceLogger&
TraceLogger::operator<< <double>(double data) {
  _sstr << std::fixed << std::setprecision(2) << data;
  return *this;
}

} // namespace helpers
