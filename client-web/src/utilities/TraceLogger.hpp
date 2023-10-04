
#pragma once

#include <cstdint>
#include <cstring> // <= strrchr()
#include <mutex>
#include <ostream>
#include <sstream> // <= std::stringstream

namespace helpers {

class TraceLogger {
public:
  static std::string getTime();

private:
  std::mutex _mutex;
  std::stringstream _sstr;

public:
  TraceLogger() = default;

public:
  void dumpOut();
  void dumpErr();
  std::string getData() const;

public:
  template <typename T>
  TraceLogger&
  operator<<(T data) {
    _sstr << data;
    return *this;
  }
};

template <> TraceLogger& TraceLogger::operator<<<bool>(bool data);

template <> TraceLogger& TraceLogger::operator<<<float>(float data);

template <> TraceLogger& TraceLogger::operator<<<double>(double data);

} // namespace helpers

#if 0

// this will reduce the "__FILE__" macro to it's filename -> friendlier to read
#define D_LOG_OUT_FILENAME \
  (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

// this is just to make the "D_LOG_OUT" macro source code easier to read
#define D_LOG_OUT_STACK D_LOG_OUT_FILENAME << "|" << __func__ << "|" << __LINE__

#define D_LOG_OUT_PREFIX \
  "[" << helpers::TraceLogger::getTime() << "] (" << D_LOG_OUT_STACK << ") -> "

#else

#define D_LOG_OUT_PREFIX "[" << helpers::TraceLogger::getTime() << "] -> "

#endif

// one line logging macro
#define D_LOG_OUT(streamMsg)                                  \
  {                                                           \
    helpers::TraceLogger tmpLogger;                           \
    tmpLogger << "LOG OUT " << D_LOG_OUT_PREFIX << streamMsg; \
    tmpLogger.dumpOut();                                      \
  }

// one line logging macro
#define D_LOG_ERR(streamMsg)                                  \
  {                                                           \
    helpers::TraceLogger tmpLogger;                           \
    tmpLogger << "LOG ERR " << D_LOG_OUT_PREFIX << streamMsg; \
    tmpLogger.dumpErr();                                      \
  }
