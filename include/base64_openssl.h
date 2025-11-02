#pragma once
#include "base64.h"

namespace cl
{
class Base64Openssl : public Base64
{
  std::string encode(const std::vector<unsigned char> &input) override;

  std::vector<unsigned char> decode(const std::string &input) override;
};
} // namespace cl
