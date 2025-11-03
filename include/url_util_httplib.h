#pragma once

#include "url_util.h"

namespace cl {
class UrlUtilHttplib : public UrlUtil {
  tl::expected<std::string, std::string> UrlEscape(
      const std::string& raw) const override;
  tl::expected<std::string, std::string> UrlUnEscape(
      const std::string& encoded) const override;
};
}  // namespace cl
