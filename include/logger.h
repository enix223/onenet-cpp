#pragma once

#include <fmt/chrono.h>
#include <fmt/core.h>

#include <iostream>
#include <string>

namespace cl {
enum class LogLevel { DEBUG, INFO, WARN, ERROR };

// Helper function to convert LogLevel to a string for output
inline std::string LogLevelToString(LogLevel level)
{
  switch (level) {
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

class Logger {
 public:
  // Constructor (optional: can set default minimum level)
  Logger(LogLevel minLevel = LogLevel::INFO) : min_level_(minLevel) {}

  // Public method to set the minimum log level
  void SetMinLevel(LogLevel level) { min_level_ = level; }

  // Template function for logging using fmt::format
  template <typename... Args>
  void Log(LogLevel level, fmt::format_string<Args...> fmt,
           Args&&... args) const
  {
    if (level < min_level_) {
      return;  // Skip messages below the minimum level
    }

    // Get the current time for the timestamp
    auto now = std::chrono::system_clock::now();

    // 1. **Timestamp**: Format time (requires fmt/chrono.h)
    std::string timestamp = fmt::format("{:%Y-%m-%d %H:%M:%S}", now);

    // 2. **Level Tag**: Convert enum to string
    std::string levelTag = LogLevelToString(level);

    // 3. **Main Message**: Use fmt::format to generate the user's message
    std::string message = fmt::format(fmt, std::forward<Args>(args)...);

    // Combine all parts into the final log entry
    std::string final_log =
        fmt::format("[{}] [{:<5}] {}", timestamp, levelTag, message);

    // Output to standard error for WARN/ERROR, otherwise standard output
    if (level == LogLevel::ERROR || level == LogLevel::WARN) {
      std::cerr << final_log << "\n";
    }
    else {
      std::cout << final_log << "\n";
    }
  }

  // Convenience wrapper methods
  template <typename... Args>
  void Debug(fmt::format_string<Args...> fmt, Args&&... args) const
  {
    Log(LogLevel::DEBUG, fmt, std::forward<Args>(args)...);
  }

  template <typename... Args>
  void Info(fmt::format_string<Args...> fmt, Args&&... args) const
  {
    Log(LogLevel::INFO, fmt, std::forward<Args>(args)...);
  }

  template <typename... Args>
  void Warn(fmt::format_string<Args...> fmt, Args&&... args) const
  {
    Log(LogLevel::WARN, fmt, std::forward<Args>(args)...);
  }

  template <typename... Args>
  void Error(fmt::format_string<Args...> fmt, Args&&... args) const
  {
    Log(LogLevel::ERROR, fmt, std::forward<Args>(args)...);
  }

 private:
  LogLevel min_level_;
};
}  // namespace cl
