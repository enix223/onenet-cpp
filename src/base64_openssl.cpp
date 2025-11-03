#include "base64_openssl.h"

#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <openssl/evp.h>

#include <algorithm>
#include <cstring>
#include <stdexcept>
#include <vector>

std::string cl::Base64Openssl::Encode(const std::vector<unsigned char>& input)
{
  // Create a memory buffer BIO
  BIO* bio = BIO_new(BIO_s_mem());
  // Create a Base64 filter BIO, set to NO_NL (no newlines)
  BIO* b64 = BIO_new(BIO_f_base64());
  BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);

  // Chain the Base64 filter to the memory buffer
  BIO_push(b64, bio);

  // Write the data through the Base64 filter
  // input.data() now returns const unsigned char*, which is ideal for BIO_write
  if (BIO_write(b64, input.data(), input.size()) <= 0) {
    BIO_free_all(b64);
    throw std::runtime_error("Base64 encode write error.");
  }

  // Ensure all data is flushed and padding is added
  BIO_flush(b64);

  // Get the memory buffer structure that holds the result
  BUF_MEM* bufferPtr;
  BIO_get_mem_ptr(b64, &bufferPtr);

  // Create the final std::string from the buffer
  std::string encoded_string(bufferPtr->data, bufferPtr->length);

  // Clean up BIOs
  BIO_free_all(b64);

  return encoded_string;
}

std::vector<unsigned char> cl::Base64Openssl::Decode(const std::string& input)
{
  // Create a Base64 filter BIO, set to NO_NL (no newlines)
  BIO* b64 = BIO_new(BIO_f_base64());
  BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);

  // Create a source memory buffer BIO and write the input string into it
  BIO* bio = BIO_new_mem_buf(input.data(), input.length());

  // Chain the Base64 filter to the memory buffer (input source)
  BIO_push(b64, bio);

  // Determine the maximum possible decoded length (input length is a good
  // estimate)
  int max_decode_len = input.length() * 3 / 4 + 1;

  // Buffer is now std::vector<unsigned char>
  std::vector<unsigned char> buffer(max_decode_len);

  // Read the data through the decoding filter
  int decoded_len = BIO_read(b64, buffer.data(), max_decode_len);

  // Clean up BIOs
  BIO_free_all(b64);

  if (decoded_len <= 0) {
    // Handle cases where decoding fails or results in empty data
    return {};
  }

  // Resize the vector to the actual decoded length before returning
  buffer.resize(decoded_len);
  return buffer;
}
