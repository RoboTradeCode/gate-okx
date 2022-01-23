#include "includes/OKXPublic.h"

OKXPublic::OKXPublic(net::io_context& ioc, const std::function<void(std::string)>& event_handler)
{
    ws = std::make_shared<WSSession>("wspap.okex.com", "8443", "/ws/v5/public?brokerId=9999", ioc, event_handler);
    ws->async_read();
}

void OKXPublic::subscribe_tickers(const std::string& inst_id)
{
    ws->write(json::serialize(json::value{
        { "op", "subscribe" },
        { "args", {
            {
                { "channel", "tickers" },
                { "instId", inst_id }
            }
        }}
    }));
}
