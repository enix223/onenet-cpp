#pragma once
#include <any>
#include <map>
#include <string>

namespace cl
{
class OneNetClient
{
public:
  OneNetClient(std::string productId, std::string productSecret, std::string deviceName, std::string deviceSecret)
      : mProductId(productId), mProductSecret(productSecret), mDeviceName(deviceName), mDeviceSecret(deviceSecret) {};

  ~OneNetClient() = default;

  void connect();

  void disconnect();

  void uploadProperties(std::map<std::string, std::any> properties);

private:
  /// @brief onenet product id
  std::string mProductId;

  /// @brief onenet product secret
  std::string mProductSecret;

  /// @brief onenet device name
  std::string mDeviceName;

  /// @brief onenet device secret
  std::string mDeviceSecret;
};
} // namespace cl
