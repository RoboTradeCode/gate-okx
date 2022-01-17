#ifndef OKEX_GATEWAY_OKEXSESSION_H
#define OKEX_GATEWAY_OKEXSESSION_H

#include <thread>
#include <iostream>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <utility>
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

auto const HOST = "wspap.okex.com";
auto const PORT = "8443";
auto const TARGET = "/ws/v5/public?brokerId=9999";

class OKExSession : public std::enable_shared_from_this<OKExSession>
{
    std::shared_ptr<websocket::stream<beast::ssl_stream<beast::tcp_stream>>> ws;
    beast::flat_buffer buffer;
    std::function<void (std::string)> handler;

    void on_read(beast::error_code ec, std::size_t bytes_transferred)
    {
        boost::ignore_unused(bytes_transferred);

        if(ec)
            throw std::runtime_error("on_read: " + ec.message());

        handler(beast::buffers_to_string(buffer.data()));

        buffer.clear();
        ws->async_read(buffer, beast::bind_front_handler(&OKExSession::on_read, shared_from_this()));
    };

public:
    explicit OKExSession(net::io_context& ioc, std::function<void (std::string)> handler)
        : handler(std::move(handler))
    {
        ssl::context ctx{ ssl::context::tls_client };
        ctx.set_verify_mode(ssl::context::verify_peer | ssl::context::verify_fail_if_no_peer_cert);
        ctx.set_default_verify_paths();
        certify::enable_native_https_server_verification(ctx);

        ws = std::make_shared<websocket::stream<beast::ssl_stream<beast::tcp_stream>>>(ioc, ctx);

        std::string host(HOST);

        tcp::resolver resolver{ioc};
        auto results = resolver.resolve(host, PORT);
        auto ep = beast::get_lowest_layer(*ws).connect(results);

        SSL_set_tlsext_host_name(ws->next_layer().native_handle(), host.c_str());
        host += ':' + std::to_string(ep.port());

        ws->next_layer().handshake(ssl::stream_base::client);

        beast::get_lowest_layer(*ws).expires_never();
        ws->set_option(websocket::stream_base::timeout::suggested(beast::role_type::client));

        ws->set_option(websocket::stream_base::decorator(
            [](websocket::request_type& req)
            {
                req.set("x-simulated-trading", "1");
            }
        ));

        ws->handshake(host, TARGET);
    }

    void write(std::string message)
    {
        ws->write(net::buffer(message));
    }

    void async_read()
    {
        ws->async_read(buffer, beast::bind_front_handler(&OKExSession::on_read, shared_from_this()));
    }
};

#endif  // OKEX_GATEWAY_OKEXSESSION_H
