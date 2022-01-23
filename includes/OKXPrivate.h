#ifndef OKX_GATEWAY_OKXPRIVATE_H
#define OKX_GATEWAY_OKXPRIVATE_H


#include <boost/beast/core.hpp>
#include <boost/json.hpp>
#include "WSSession.h"
#include "utils.h"

namespace net = boost::asio;
namespace json = boost::json;

class OKXPrivate
{
    std::shared_ptr<WSSession> ws;

public:
    explicit OKXPrivate(boost::asio::io_context& ioc, const std::function<void(std::string)>& event_handler);

    void login(const std::string& api_key, const std::string& passphrase, const std::string& secret_key);

    void order(const std::string& id, const std::string& side, const std::string& inst_id, const std::string& sz,
        const std::string& px, const std::string& td_mode = "cash", const std::string& ord_type = "limit");

    void cancel_order(const std::string& id, const std::string& inst_id, const std::string& ord_id);

    void subscribe_balance_and_position();

    void subscribe_orders(const std::string& inst_type = "SPOT");
};


#endif  // OKX_GATEWAY_OKXPRIVATE_H
