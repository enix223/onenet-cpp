#pragma once

#include <string>
#include <vector>

namespace cl {
class Base64 {
 public:
  virtual std::string Encode(const std::vector<unsigned char>& input) = 0;

  virtual std::vector<unsigned char> Decode(const std::string& input) = 0;
};
}  // namespace cl
