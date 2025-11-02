#pragma once

#include <fmt/chrono.h>
#include <fmt/core.h>
#include <iostream>
#include <string>

namespace cl
{
enum class LogLevel
{
  DEBUG,
  INFO,
  WARN,
  ERROR
};

// Helper function to convert LogLevel to a string for output
inline std::string logLevelToString(LogLevel level)
{
  switch (level)
  {
  case LogLevel::DEBUG:
    return "DEBUG";
  case LogLevel::INFO:
    return "INFO";
  case LogLevel::WARN:
    return "WARN";
  case LogLevel::ERROR:
    return "ERROR";
  default:
    return "UNKNOWN";
  }
}

class Logger
{
public:
  // Constructor (optional: can set default minimum level)
  Logger(LogLevel minLevel = LogLevel::INFO) : minLevel_(minLevel)
  {
  }

  // Public method to set the minimum log level
  void setMinLevel(LogLevel level)
  {
    minLevel_ = level;
  }

  // Template function for logging using fmt::format
  template <typename... Args> void log(LogLevel level, fmt::format_string<Args...> fmt, Args &&...args)
  {
    if (level < minLevel_)
    {
      return; // Skip messages below the minimum level
    }

    // Get the current time for the timestamp
    auto now = std::chrono::system_clock::now();

    // 1. **Timestamp**: Format time (requires fmt/chrono.h)
    std::string timestamp = fmt::format("{:%Y-%m-%d %H:%M:%S}", now);

    // 2. **Level Tag**: Convert enum to string
    std::string levelTag = logLevelToString(level);

    // 3. **Main Message**: Use fmt::format to generate the user's message
    std::string message = fmt::format(fmt, std::forward<Args>(args)...);

    // Combine all parts into the final log entry
    std::string final_log = fmt::format("[{}] [{:<5}] {}", timestamp, levelTag, message);

    // Output to standard error for WARN/ERROR, otherwise standard output
    if (level == LogLevel::ERROR || level == LogLevel::WARN)
    {
      std::cerr << final_log << "\n";
    }
    else
    {
      std::cout << final_log << "\n";
    }
  }

  // Convenience wrapper methods
  template <typename... Args> void debug(fmt::format_string<Args...> fmt, Args &&...args)
  {
    log(LogLevel::DEBUG, fmt, std::forward<Args>(args)...);
  }

  template <typename... Args> void info(fmt::format_string<Args...> fmt, Args &&...args)
  {
    log(LogLevel::INFO, fmt, std::forward<Args>(args)...);
  }

  template <typename... Args> void warn(fmt::format_string<Args...> fmt, Args &&...args)
  {
    log(LogLevel::WARN, fmt, std::forward<Args>(args)...);
  }

  template <typename... Args> void error(fmt::format_string<Args...> fmt, Args &&...args)
  {
    log(LogLevel::ERROR, fmt, std::forward<Args>(args)...);
  }

private:
  LogLevel minLevel_;
};
} // namespace cl
