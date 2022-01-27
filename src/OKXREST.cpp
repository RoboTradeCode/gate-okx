#include "includes/OKXREST.h"

OKXREST::OKXREST(boost::asio::io_context& ioc, std::string api_key, std::string passphrase, std::string secret_key)
    : api_key(std::move(api_key)),
      passphrase(std::move(passphrase)),
      secret_key(std::move(secret_key))
{
    s = std::make_shared<HTTPSession>("www.okx.com", "443", ioc);
}

std::string OKXREST::get_order_list()
{
    return s->get("/api/v5/trade/orders-pending", api_key, passphrase, secret_key);
}

std::string OKXREST::get_balance()
{
    return s->get("/api/v5/account/balance", api_key, passphrase, secret_key);
}
