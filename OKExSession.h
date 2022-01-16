#ifndef OKEX_GATEWAY_OKEXSESSION_H
#define OKEX_GATEWAY_OKEXSESSION_H

#include <thread>
#include <iostream>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/certify/extensions.hpp>
#include <boost/certify/https_verification.hpp>
#include <boost/json/src.hpp>

namespace beast = boost::beast;
namespace websocket = beast::websocket;
namespace net = boost::asio;
namespace ssl = boost::asio::ssl;
using tcp = boost::asio::ip::tcp;
namespace certify = boost::certify;
namespace json = boost::json;

void fail(beast::error_code ec, char const* what)
{
    std::cerr << what << ": " << ec.message() << std::endl;
}

class OKExSession : public std::enable_shared_from_this<OKExSession>
{
    websocket::stream<beast::ssl_stream<beast::tcp_stream>> ws;
    beast::flat_buffer buffer;
    std::function<void (std::string)> callback;

    void on_read(beast::error_code ec, std::size_t bytes_transferred)
    {
        boost::ignore_unused(bytes_transferred);

        if(ec)
            return fail(ec, "read");

        callback(beast::buffers_to_string(buffer.data()));

        buffer.clear();
        ws.async_read(buffer, beast::bind_front_handler(&OKExSession::on_read, shared_from_this()));
    };

public:
    explicit OKExSession(net::io_context& ioc, ssl::context& ctx, std::function<void (std::string)> callback)
        : ws(net::make_strand(ioc), ctx),
          callback(std::move(callback))
    {
        std::string host = "wspap.okex.com";
        std::string port = "8443";
        std::string target = "/ws/v5/public?brokerId=9999";

        tcp::resolver resolver{ioc};
        auto results = resolver.resolve(host, port);
        auto ep = beast::get_lowest_layer(ws).connect(results);

        SSL_set_tlsext_host_name(ws.next_layer().native_handle(), host.c_str());
        host += ':' + std::to_string(ep.port());

        ws.next_layer().handshake(ssl::stream_base::client);

        beast::get_lowest_layer(ws).expires_never();
        ws.set_option(websocket::stream_base::timeout::suggested(beast::role_type::client));

        ws.set_option(websocket::stream_base::decorator(
            [](websocket::request_type& req)
            {
                req.set("x-simulated-trading", "1");
            }
        ));

        ws.handshake(host, target);

        json::value jv = {
            { "op", "subscribe" },
            { "args", {
                {
                    { "channel", "tickers" },
                    { "instId", "BTC-USDT" },
                }
            }}
        };

        ws.write(net::buffer(json::serialize(jv)));
    }

    void run()
    {
        ws.async_read(buffer, beast::bind_front_handler(&OKExSession::on_read, shared_from_this()));
    };
};

#endif  // OKEX_GATEWAY_OKEXSESSION_H
