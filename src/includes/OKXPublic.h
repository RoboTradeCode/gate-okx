#ifndef OKX_GATEWAY_OKXPUBLIC_H
#define OKX_GATEWAY_OKXPUBLIC_H


#include <boost/beast/core.hpp>
#include <boost/json.hpp>
#include "WSSession.h"

namespace net = boost::asio;
namespace json = boost::json;

class OKXPublic
{
    std::shared_ptr<WSSession> ws;

public:
    explicit OKXPublic(boost::asio::io_context& ioc, const std::function<void(std::string)>& event_handler);

    void subscribe_tickers(const std::string& inst_id);
};


#endif  // OKX_GATEWAY_OKXPUBLIC_H
