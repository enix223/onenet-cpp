#include "url_util_httplib.h"

#include <httplib.h>

tl::expected<std::string, std::string> cl::UrlUtilHttplib::UrlEscape(
    const std::string& raw) const
{
  return httplib::encode_uri_component(raw);
}

tl::expected<std::string, std::string> cl::UrlUtilHttplib::UrlUnEscape(
    const std::string& encoded) const
{
  return httplib::decode_uri_component(encoded);
}