#pragma once

#include <any>
#include <map>
#include <string>

#include "logger.h"
#include "mqtt/client.h"

namespace cl {
class OneNetClient {
 public:
  static const std::string SERVER_URL;
  static const std::string CA_CERT_PATH;
  static const std::string PERSIST_DIR;

  OneNetClient(bool deviceLevelAuth, std::string productId,
               std::string productSecret, std::string deviceName,
               std::string deviceSecret);

  ~OneNetClient() = default;

  void Connect();

  void Disconnect();

  void UploadProperties(std::map<std::string, std::any> properties);

 private:
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

  bool device_level_auth_;

  std::string BuildToken() const;

  std::string BytesTohex(const std::vector<unsigned char>& bytes) const;

  std::string HmacSha1(const std::string& key,
                       const std::string& message) const;
};
}  // namespace cl
