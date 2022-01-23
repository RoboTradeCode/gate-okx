#ifndef OKX_GATEWAY_UTILS_H
#define OKX_GATEWAY_UTILS_H


#include <array>
#include <openssl/hmac.h>
#include <boost/beast.hpp>

std::string base64_hmac_sha256(const std::string& message, const std::string& secret_key);


#endif  // OKX_GATEWAY_UTILS_H
