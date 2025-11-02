#pragma once

#include "logger.h"
#include "mqtt/client.h"
#include <any>
#include <map>
#include <string>

namespace cl
{
class OneNetClient
{
public:
  static const std::string SERVER_URL;
  static const std::string CA_CERT_PATH;
  static const std::string PERSIST_DIR;

  OneNetClient(bool deviceLevelAuth, std::string productId, std::string productSecret, std::string deviceName,
               std::string deviceSecret);

  ~OneNetClient() = default;

  void connect();

  void disconnect();

  void uploadProperties(std::map<std::string, std::any> properties);

private:
  /// @brief mqtt client
  mqtt::client mClient;

  /// @brief onenet product id
  std::string mProductId;

  /// @brief onenet product secret
  std::string mProductSecret;

  /// @brief onenet device name
  std::string mDeviceName;

  /// @brief onenet device secret
  std::string mDeviceSecret;

  /// @brief logger
  cl::Logger mLogger;

  bool mDeviceLevelAuth;

  std::string buildToken() const;

  std::string bytesTohex(const std::vector<unsigned char> &bytes) const;

  std::string cl::OneNetClient::hmacSha1(const std::string &key, const std::string &message) const;

  std::string cl::OneNetClient::base64Encode(const std::vector<unsigned char> &bytes) const;
};
} // namespace cl
