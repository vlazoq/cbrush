#include "cbrush/logger.h"

#include <array>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace cbrush {

Logger& Logger::instance() {
  static Logger g;
  return g;
}

static const char* level_name(LogLevel l) {
  switch (l) {
    case LogLevel::Trace: return "TRACE";
    case LogLevel::Debug: return "DEBUG";
    case LogLevel::Info:  return "INFO ";
    case LogLevel::Warn:  return "WARN ";
    case LogLevel::Error: return "ERROR";
    case LogLevel::Fatal: return "FATAL";
  }
  return "?????";
}

static std::string iso_now() {
  using namespace std::chrono;
  auto now = system_clock::now();
  auto t   = system_clock::to_time_t(now);
  auto ms  = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;
  std::tm tm{};
#if defined(_WIN32)
  gmtime_s(&tm, &t);
#else
  gmtime_r(&t, &tm);
#endif
  std::ostringstream os;
  os << std::put_time(&tm, "%Y-%m-%dT%H:%M:%S")
     << '.' << std::setfill('0') << std::setw(3) << ms.count() << 'Z';
  return os.str();
}

void Logger::log(LogLevel lvl, std::string_view component, std::string_view msg) {
  if (static_cast<int>(lvl) < static_cast<int>(level_)) return;
  std::lock_guard<std::mutex> lk(mu_);
  std::ostream& os = out_ ? *out_ : std::cerr;
  os << iso_now() << ' ' << level_name(lvl) << ' ' << component << " - " << msg << '\n';
}

}  // namespace cbrush
