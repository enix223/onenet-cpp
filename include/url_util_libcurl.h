#if 0

#include <curl/curl.h>

#include <memory>

#include "url_util.h"

namespace cl {

// Alias for the pointer type (readability)
using CurlHandle = CURL*;

// The shared_ptr takes the pointer type (CurlHandle) and the deleter type.
using SmartCurlHandle = std::shared_ptr<CURL>;

class UrlUtilCurl : public UrlUtil {
 public:
  explicit UrlUtilCurl();
  tl::expected<std::string, std::string> UrlEscape(
      const std::string& raw) const override;
  tl::expected<std::string, std::string> UrlUnEscape(
      const std::string& encoded) const override;

 private:
  SmartCurlHandle curl_;
};
}  // namespace cl

#endif