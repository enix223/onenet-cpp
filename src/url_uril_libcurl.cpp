#include "url_uril_libcurl.h"

auto CurlDeleter = [](cl::CurlHandle handle) {
  if (handle) {
    curl_easy_cleanup(handle);
  }
};

cl::UrlUtilCurl::UrlUtilCurl() : curl_(curl_easy_init(), CurlDeleter) {}

tl::expected<std::string, std::string> cl::UrlUtilCurl::UrlEscape(
    const std::string& raw) const
{
  char* output_buffer =
      curl_easy_escape(curl_.get(), raw.c_str(), raw.length());

  if (!output_buffer) {
    return tl::make_unexpected<std::string>("unable to escape");
  }

  std::string escaped_string(output_buffer);
  curl_free(output_buffer);

  return escaped_string;
}

tl::expected<std::string, std::string> cl::UrlUtilCurl::UrlUnEscape(
    const std::string& encoded) const
{
  char* decoded_buffer =
      curl_easy_unescape(curl_.get(), encoded.c_str(), encoded.length(), NULL);
  if (!decoded_buffer) {
    return tl::make_unexpected<std::string>("unable to unescape");
  }

  std::string decoded_string(decoded_buffer);
  curl_free(decoded_buffer);

  return decoded_string;
}
