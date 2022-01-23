#ifndef OKX_GATEWAY_HTTPSESSION_H
#define OKX_GATEWAY_HTTPSESSION_H


#include <boost/beast/http.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/certify/https_verification.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "utils.h"

namespace net = boost::asio;
namespace ssl = boost::asio::ssl;
namespace beast = boost::beast;
namespace http = beast::http;
using tcp = boost::asio::ip::tcp;

class HTTPSession
{
    std::string host;
    std::shared_ptr<beast::ssl_stream<beast::tcp_stream>> stream;

public:
    explicit HTTPSession(const std::string& host, const std::string& port, net::io_context& ioc);

    std::string get(const std::string& target, const std::string& api_key, const std::string& passphrase,
        const std::string& secret_key);
};


#endif  // OKX_GATEWAY_HTTPSESSION_H
