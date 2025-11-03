#pragma once
#include "base64.h"

namespace cl {
class Base64Openssl : public Base64 {
  std::string Encode(const std::vector<unsigned char>& input) override;

  std::vector<unsigned char> Decode(const std::string& input) override;
};
}  // namespace cl
