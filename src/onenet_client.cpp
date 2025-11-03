#include "onenet_client.h"

#include <mqtt/client.h>
#include <openssl/hmac.h>

#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

const std::string cl::OneNetClient::kServerUrl{
    "mqtts://mqttstls.heclouds.com:8883"};
const std::string cl::OneNetClient::kCaCert = R"(-----BEGIN CERTIFICATE-----
MIIDOzCCAiOgAwIBAgIJAPCCNfxANtVEMA0GCSqGSIb3DQEBCwUAMDQxCzAJBgNV
BAYTAkNOMQ4wDAYDVQQKDAVDTUlPVDEVMBMGA1UEAwwMT25lTkVUIE1RVFRTMB4X
DTE5MDUyOTAxMDkyOFoXDTQ5MDUyMTAxMDkyOFowNDELMAkGA1UEBhMCQ04xDjAM
BgNVBAoMBUNNSU9UMRUwEwYDVQQDDAxPbmVORVQgTVFUVFMwggEiMA0GCSqGSIb3
DQEBAQUAA4IBDwAwggEKAoIBAQC/VvJ6lGWfy9PKdXKBdzY83OERB35AJhu+9jkx
5d4SOtZScTe93Xw9TSVRKrFwu5muGgPusyAlbQnFlZoTJBZY/745MG6aeli6plpR
r93G6qVN5VLoXAkvqKslLZlj6wXy70/e0GC0oMFzqSP0AY74icANk8dUFB2Q8usS
UseRafNBcYfqACzF/Wa+Fu/upBGwtl7wDLYZdCm3KNjZZZstvVB5DWGnqNX9HkTl
U9NBMS/7yph3XYU3mJqUZxryb8pHLVHazarNRppx1aoNroi+5/t3Fx/gEa6a5PoP
ouH35DbykmzvVE67GUGpAfZZtEFE1e0E/6IB84PE00llvy3pAgMBAAGjUDBOMB0G
A1UdDgQWBBTTi/q1F2iabqlS7yEoX1rbOsz5GDAfBgNVHSMEGDAWgBTTi/q1F2ia
bqlS7yEoX1rbOsz5GDAMBgNVHRMEBTADAQH/MA0GCSqGSIb3DQEBCwUAA4IBAQAL
aqJ2FgcKLBBHJ8VeNSuGV2cxVYH1JIaHnzL6SlE5q7MYVg+Ofbs2PRlTiWGMazC7
q5RKVj9zj0z/8i3ScWrWXFmyp85ZHfuo/DeK6HcbEXJEOfPDvyMPuhVBTzuBIRJb
41M27NdIVCdxP6562n6Vp0gbE8kN10q+ksw8YBoLFP0D1da7D5WnSV+nwEIP+F4a
3ZX80bNt6tRj9XY0gM68mI60WXrF/qYL+NUz+D3Lw9bgDSXxpSN8JGYBR85BxBvR
NNAhsJJ3yoAvbPUQ4m8J/CoVKKgcWymS1pvEHmF47pgzbbjm5bdthlIx+swdiGFa
WzdhzTYwVkxBaU+xf/2w
-----END CERTIFICATE-----)";
const std::string cl::OneNetClient::kSigningMethod = "sha1";
const std::string cl::OneNetClient::kSigningAlgVersion = "2018-10-31";

cl::OneNetClient::OneNetClient(bool deviceLevelAuth, std::string productId,
                               std::string productSecret,
                               std::string deviceName, std::string deviceSecret,
                               std::shared_ptr<cl::Base64> base64,
                               std::shared_ptr<cl::UrlUtil> urlUtil)
    : device_level_auth_(deviceLevelAuth),
      product_id_(productId),
      product_secret_(productSecret),
      device_name_(deviceName),
      device_secret_(deviceSecret),
      logger_{LogLevel::DEBUG},
      mqtt_client_{kServerUrl, deviceName},
      base64_(base64),
      urlUtil_(urlUtil)
{
}

void cl::OneNetClient::Connect()
{
  logger_.Info("start connecting");

  auto caFile = BuildCaFile(kCaCert);
  if (!caFile.has_value()) {
    logger_.Error("failed to create ca file: {}", caFile.error());
    return;
  }

  // ssl options
  auto sslopts =
      mqtt::ssl_options_builder().trust_store(caFile.value()).finalize();

  // connect options
  auto token = BuildToken();
  if (!token.has_value()) {
    logger_.Error("failed to connect: {}", token.error());
    return;
  }
  logger_.Debug("token = {}", token.value());
  auto connOpts = mqtt::connect_options_builder()
                      .automatic_reconnect(true)
                      // MQTT 3.1.1
                      .mqtt_version(4)
                      .ssl(std::move(sslopts))
                      .user_name(product_id_)
                      .password(token.value())
                      .finalize();

  try {
    auto rsp = mqtt_client_.connect(std::move(connOpts));
    logger_.Info("connect result: {}",
                 rsp.is_session_present() ? "ok" : "fail");
  } catch (mqtt::exception& e) {
    logger_.Error("failed to connect: [client id = {} , error = {}]",
                  mqtt_client_.get_client_id(),
                  e.printable_error(e.get_return_code(), e.get_reason_code(),
                                    e.get_message()));
  }
}

void cl::OneNetClient::Disconnect() {}

void cl::OneNetClient::UploadProperties(
    std::map<std::string, std::any> properties)
{
}

tl::expected<std::string, std::string> cl::OneNetClient::BuildCaFile(
    const std::string& content) const
{
  const auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
                       std::chrono::system_clock::now().time_since_epoch())
                       .count();
  const std::string filename = fmt::format("/dev/shm/onenet_ca_{}", now);

  std::ofstream caFile{filename};
  if (!caFile.is_open()) {
    return tl::make_unexpected<std::string>("failed to create ca file");
  }

  caFile << content;
  caFile.close();

  return filename;
}

tl::expected<std::string, std::string> cl::OneNetClient::BuildToken() const
{
  auto expire = std::chrono::system_clock::now() + std::chrono::hours(8760);
  auto et = std::to_string(std::chrono::duration_cast<std::chrono::seconds>(
                               expire.time_since_epoch())
                               .count());
  auto res = device_level_auth_ ? fmt::format("products/{}/devices/{}",
                                              product_id_, device_name_)
                                : fmt::format("products/{}", product_id_);
  auto secret = device_level_auth_ ? device_secret_ : product_secret_;
  auto secretBytes = base64_->Decode(secret);
  if (secretBytes.size() == 0) {
    return tl::make_unexpected<std::string>("failed to decode secret");
  }
  auto pending =
      et + "\n" + kSigningMethod + "\n" + res + "\n" + kSigningAlgVersion;
  logger_.Debug("string to sign = {}", pending);
  auto sig_raw = HmacSha1(secret, pending);
  auto signature = base64_->Encode(sig_raw);
  logger_.Debug("signature = {}", signature);

  auto versionEscaped = urlUtil_->UrlEscape(kSigningAlgVersion);
  auto resEscape = urlUtil_->UrlEscape(res);
  auto etEscape = urlUtil_->UrlEscape(et);
  auto signingMethodEscape = urlUtil_->UrlEscape(kSigningMethod);
  auto signatureEscape = urlUtil_->UrlEscape(signature);
  if (!versionEscaped.has_value() || !resEscape.has_value() ||
      !etEscape.has_value() || !signingMethodEscape.has_value() ||
      !signatureEscape.has_value()) {
    return tl::make_unexpected<std::string>("failed to do url escape");
  }
  auto token =
      fmt::format("version={}&res={}&et={}&method={}&sign={}",
                  versionEscaped.value(), resEscape.value(), etEscape.value(),
                  signingMethodEscape.value(), signatureEscape.value());
  return token;
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

std::vector<unsigned char> cl::OneNetClient::HmacSha1(
    const std::string& key, const std::string& message) const
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

  return digest;
}
