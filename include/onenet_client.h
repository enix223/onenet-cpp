#pragma once

#include <any>
#include <map>
#include <string>

#include "base64.h"
#include "logger.h"
#include "mqtt/client.h"

namespace cl {
class OneNetClient {
 public:
  static const std::string kServerUrl;
  static const std::string kCaCertPath;
  static const std::string kSigningMethod;
  static const std::string kSigningAlgVersion;

  OneNetClient(bool deviceLevelAuth, std::string productId,
               std::string productSecret, std::string deviceName,
               std::string deviceSecret, std::shared_ptr<cl::Base64> base64);

  ~OneNetClient() = default;

  void Connect();

  void Disconnect();

  void UploadProperties(std::map<std::string, std::any> properties);

 private:
  std::shared_ptr<cl::Base64> base64_;

  /// @brief mqtt client
  mqtt::client mqtt_client_;

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

  std::string BuildToken() const;

  std::string BytesTohex(const std::vector<unsigned char>& bytes) const;

  std::string HmacSha1(const std::string& key,
                       const std::string& message) const;
};
}  // namespace cl
