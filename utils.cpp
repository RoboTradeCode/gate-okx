#include "includes/utils.h"

std::string base64_hmac_sha256(const std::string& message, const std::string& secret_key)
{
    std::array<unsigned char, EVP_MAX_MD_SIZE> hash{};
    unsigned int hash_len;

    HMAC(
        EVP_sha256(),
        secret_key.data(),
        static_cast<int>(secret_key.size()),
        reinterpret_cast<unsigned char const*>(message.data()),
        static_cast<int>(message.size()),
        hash.data(),
        &hash_len
    );

    std::array<unsigned char, EVP_MAX_MD_SIZE> base64{};
    boost::beast::detail::base64::encode(&base64, hash.data(), hash_len);

    return std::string{ reinterpret_cast<char const*>(base64.data()) };
}
