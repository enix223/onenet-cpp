#include "onenet_client.h"

#include <mqtt/client.h>
#include <openssl/hmac.h>

#include <chrono>
#include <iomanip>

const std::string cl::OneNetClient::SERVER_URL{
    "mqtts://mqttstls.heclouds.com:8883"};
const std::string cl::OneNetClient::CA_CERT_PATH = "onenet-ca.pem";

cl::OneNetClient::OneNetClient(bool deviceLevelAuth, std::string productId,
                               std::string productSecret,
                               std::string deviceName, std::string deviceSecret)
    : device_level_auth_(deviceLevelAuth),
      product_id_(productId),
      product_secret_(productSecret),
      device_name_(deviceName),
      device_secret_(deviceSecret),
      logger_{LogLevel::DEBUG},
      mqtt_client_{SERVER_URL, deviceName}
{
}

void cl::OneNetClient::Connect()
{
  logger_.Info("start connecting");

  // ssl options
  auto sslopts = mqtt::ssl_options_builder()
                     .trust_store(CA_CERT_PATH)  // set ca cert trust store
                     .enable_server_cert_auth(true)
                     .finalize();

  // connect options
  auto token = BuildToken();
  auto connOpts = mqtt::connect_options_builder()
                      .automatic_reconnect(true)
                      .clean_session(true)
                      .ssl(sslopts)
                      .user_name(product_id_)
                      .password(token)
                      .finalize();

  try {
    auto rsp = mqtt_client_.connect(connOpts);
    logger_.Info("connect result: {}",
                 rsp.is_session_present() ? "ok" : "fail");
  } catch (mqtt::exception& e) {
    logger_.Error("failed to connect: {}", e.get_message());
  }
}

void cl::OneNetClient::Disconnect() {}

void cl::OneNetClient::UploadProperties(
    std::map<std::string, std::any> properties)
{
}

std::string cl::OneNetClient::BuildToken() const
{
  auto expire = std::chrono::system_clock::now() + std::chrono::hours(8760);
  auto et = std::chrono::duration_cast<std::chrono::seconds>(
                expire.time_since_epoch())
                .count();
  auto res = device_level_auth_ ? fmt::format("products/{}/devices/{}",
                                              product_id_, device_name_)
                                : fmt::format("products/{}", product_id_);
  auto secret = device_level_auth_ ? device_secret_ : product_secret_;

  return "";
}

std::string cl::OneNetClient::BytesTohex(
    const std::vector<unsigned char>& bytes) const
{
  std::stringstream ss;
  ss << std::hex << std::setfill('0');
  for (const auto& byte : bytes) {
    // Output each byte as two hexadecimal characters
    ss << std::setw(2) << static_cast<int>(byte);
  }
  return ss.str();
}

std::string cl::OneNetClient::HmacSha1(const std::string& key,
                                       const std::string& message) const
{
  // 1. Determine the required buffer size for the output (SHA1 is 20 bytes/160
  // bits) EVP_MAX_MD_SIZE is a safe upper limit, but SHA1_DIGEST_LENGTH (20) is
  // specific.
  unsigned int digest_len = EVP_MAX_MD_SIZE;
  std::vector<unsigned char> digest(digest_len);

  // 2. Call the simplified OpenSSL HMAC function
  // HMAC(EVP_sha1(), key_data, key_length, message_data, message_length,
  // output_buffer, output_length_ptr)
  HMAC(EVP_sha1(), key.c_str(), static_cast<int>(key.length()),
       reinterpret_cast<const unsigned char*>(message.c_str()),
       static_cast<int>(message.length()), digest.data(), &digest_len);

  // 3. Resize the vector to the actual digest length (20 bytes for SHA1)
  digest.resize(digest_len);

  // 4. Convert the raw bytes to a hex string for easy reading
  return BytesTohex(digest);
}
