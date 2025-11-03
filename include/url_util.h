#pragma once
#include <string>
#include <tl/expected.hpp>

namespace cl {
class UrlUtil {
 public:
  virtual tl::expected<std::string, std::string> UrlEscape(
      const std::string& raw) const = 0;
  virtual tl::expected<std::string, std::string> UrlUnEscape(
      const std::string& encoded) const = 0;
};
}  // namespace cl
