#include "OKXPrivate.h"

OKXPrivate::OKXPrivate(net::io_context& ioc, const std::function<void(std::string)>& event_handler,
    std::string api_key, std::string passphrase, std::string secret_key)
    : api_key(std::move(api_key)),
      passphrase(std::move(passphrase)),
      secret_key(std::move(secret_key))
{
    ws = std::make_shared<WSSession>("ws.okx.com", "8443", "/ws/v5/private", ioc, event_handler);
    ws->async_read();
}

void OKXPrivate::login()
{
    auto timestamp = std::to_string(time(nullptr));
    auto sign = base64_hmac_sha256(timestamp + "GET" + "/users/self/verify", secret_key);

    ws->write(json::serialize(json::value{
        { "op", "login" },
        { "args", {
            {
                { "apiKey", api_key },
                { "passphrase", passphrase },
                { "timestamp", timestamp },
                { "sign", sign }
            }
        }}
    }));
}

void OKXPrivate::order(const std::string& id, const std::string& side, const std::string& inst_id,
    const std::string& sz, const std::string& px, const std::string& td_mode, const std::string& ord_type)
{
    ws->write(json::serialize(json::value{
        { "id", id },
        { "op", "order" },
        { "args", {
            {
                { "side", side },
                { "instId", inst_id },
                { "tdMode", td_mode },
                { "ordType", ord_type },
                { "sz", sz },
                { "px", px }
            }
        }}
    }));
}

void OKXPrivate::cancel_order(const std::string& id, const std::string& inst_id, const std::string& ord_id)
{
    ws->write(json::serialize(json::value{
        { "id", id },
        { "op", "cancel-order" },
        { "args", {
            {
                { "instId", inst_id },
                { "ordId", ord_id },
            }
        }}
    }));
}

void OKXPrivate::subscribe_balance_and_position()
{
    ws->write(json::serialize(json::value{
        { "op", "subscribe" },
        { "args", {
            {
                { "channel", "balance_and_position" },
            }
        }}
    }));
}

void OKXPrivate::subscribe_orders(const std::string& inst_type)
{
    ws->write(json::serialize(json::value{
        { "op", "subscribe" },
        { "args", {
            {
                { "channel", "orders" },
                { "instType", inst_type }
            }
        }}
    }));
}
