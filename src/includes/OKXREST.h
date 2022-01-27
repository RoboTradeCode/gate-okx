#ifndef OKX_GATEWAY_OKXREST_H
#define OKX_GATEWAY_OKXREST_H


#include "HTTPSession.h"

class OKXREST
{
    std::shared_ptr<HTTPSession> s;
    std::string api_key;
    std::string passphrase;
    std::string secret_key;

public:
    explicit OKXREST(boost::asio::io_context& ioc, std::string api_key, std::string passphrase, std::string secret_key);

    std::string get_order_list();

    std::string get_balance();
};


#endif  // OKX_GATEWAY_OKXREST_H
