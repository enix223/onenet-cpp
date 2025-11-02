#pragma once

#include <string>
#include <vector>

namespace cl
{
class Base64
{
public:
  virtual std::string encode(const std::vector<unsigned char> &input) = 0;

  virtual std::vector<unsigned char> decode(const std::string &input) = 0;
};
} // namespace cl
