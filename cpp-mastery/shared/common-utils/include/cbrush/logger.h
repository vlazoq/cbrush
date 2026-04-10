#pragma once
// cbrush::Logger — tiny thread-safe structured logger.
// Formats: "<ISO-time> <LEVEL> <component> - <message>"

#include <mutex>
#include <ostream>
#include <sstream>
#include <string>
#include <string_view>

namespace cbrush {

enum class LogLevel { Trace, Debug, Info, Warn, Error, Fatal };

class Logger {
 public:
  static Logger& instance();

  void set_level(LogLevel lvl) noexcept { level_ = lvl; }
  void set_sink(std::ostream* out) noexcept { out_ = out; }

  void log(LogLevel lvl, std::string_view component, std::string_view msg);

  // sugar
  void info(std::string_view c, std::string_view m)  { log(LogLevel::Info,  c, m); }
  void warn(std::string_view c, std::string_view m)  { log(LogLevel::Warn,  c, m); }
  void error(std::string_view c, std::string_view m) { log(LogLevel::Error, c, m); }
  void debug(std::string_view c, std::string_view m) { log(LogLevel::Debug, c, m); }

 private:
  Logger() = default;
  std::mutex mu_;
  LogLevel level_ = LogLevel::Info;
  std::ostream* out_ = nullptr;  // defaults to std::cerr in logger.cpp
};

// build a message piecewise:  LOG_INFO("net", "accepted fd=" << fd);
#define CBRUSH_LOG(level, component, expr)                                 \
  do {                                                                     \
    std::ostringstream _cbrush_os;                                         \
    _cbrush_os << expr;                                                    \
    ::cbrush::Logger::instance().log((level), (component), _cbrush_os.str()); \
  } while (0)

#define LOG_INFO(c, expr)  CBRUSH_LOG(::cbrush::LogLevel::Info,  c, expr)
#define LOG_WARN(c, expr)  CBRUSH_LOG(::cbrush::LogLevel::Warn,  c, expr)
#define LOG_ERROR(c, expr) CBRUSH_LOG(::cbrush::LogLevel::Error, c, expr)
#define LOG_DEBUG(c, expr) CBRUSH_LOG(::cbrush::LogLevel::Debug, c, expr)

}  // namespace cbrush
