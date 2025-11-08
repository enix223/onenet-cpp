#pragma once

#include <any>
#include <atomic>
#include <map>
#include <memory>
#include <string>
#include <tl/expected.hpp>

#include "any.h"
#include "base64.h"
#include "logger.h"
#include "mqtt/client.h"
#include "url_util.h"

namespace cl {
class OneNetClient {
 public:
  static const std::string kServerUrl;
  static const std::string kCaCert;
  static const std::string kSigningMethod;
  static const std::string kSigningAlgVersion;

  OneNetClient(bool deviceLevelAuth, std::string productId,
               std::string productSecret, std::string deviceName,
               std::string deviceSecret, std::shared_ptr<cl::Base64> base64,
               std::shared_ptr<cl::UrlUtil> urlUtil);

  ~OneNetClient() = default;

  void Connect();

  void Disconnect();

  void UploadProperties(std::map<std::string, cl::Any> properties);

 private:
  std::shared_ptr<cl::Base64> base64_;
  std::shared_ptr<cl::UrlUtil> urlUtil_;

  /// @brief mqtt client
  mqtt::async_client mqtt_client_;

  /// @brief onenet product id
  std::string product_id_;

  /// @brief onenet product secret
  std::string product_secret_;

  /// @brief onenet device name
  std::string device_name_;

  /// @brief onenet device secret
  std::string device_secret_;

  /// @brief logger
  cl::Logger logger_;

  /// @brief enable device level auth or product level auth
  bool device_level_auth_;

  /// @brief internal thread
  std::unique_ptr<std::thread> worker_thread_;

  tl::expected<std::string, std::string> BuildCaFile(
      const std::string& content) const;

  tl::expected<std::string, std::string> BuildToken() const;

  std::vector<unsigned char> HmacSha1(
      const std::vector<unsigned char>& secretBytes,
      const std::string& message) const;

  void RunLoop();
};
}  // namespace cl
