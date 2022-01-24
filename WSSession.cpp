#include "includes/WSSession.h"

void WSSession::on_read(beast::error_code ec, std::size_t bytes_transferred)
{
    boost::ignore_unused(bytes_transferred);

    if (ec)
        throw std::runtime_error("on_read: " + ec.message());

    event_handler(beast::buffers_to_string(buffer.data()));

    buffer.clear();
    ws->async_read(buffer, beast::bind_front_handler(&WSSession::on_read, shared_from_this()));
}

WSSession::WSSession(std::string host, const std::string& port, const std::string& target, net::io_context& ioc,
    std::function<void(std::string)> event_handler)
    : event_handler(std::move(event_handler))
{
    ssl::context ctx{ ssl::context::tls_client };
    ctx.set_verify_mode(ssl::context::verify_peer | ssl::context::verify_fail_if_no_peer_cert);
    ctx.set_default_verify_paths();

    ws = std::make_shared<websocket::stream<beast::ssl_stream<beast::tcp_stream>>>(ioc, ctx);

    tcp::resolver resolver{ ioc };
    auto results = resolver.resolve(host, port);
    auto ep = beast::get_lowest_layer(*ws).connect(results);

    SSL_set_tlsext_host_name(ws->next_layer().native_handle(), host.c_str());
    host += ':' + std::to_string(ep.port());

    ws->next_layer().handshake(ssl::stream_base::client);

    beast::get_lowest_layer(*ws).expires_never();
    ws->set_option(websocket::stream_base::stream_base::timeout{
        std::chrono::seconds(30),
        std::chrono::seconds(30),
        true
    });

    // TODO: Remove on production
    ws->set_option(websocket::stream_base::decorator(
        [](websocket::request_type& req)
        {
            req.set("x-simulated-trading", "1");
        }
    ));

    ws->handshake(host, target);
}

void WSSession::write(const std::string& message)
{
    ws->write(net::buffer(message));
}

void WSSession::async_read()
{
    ws->async_read(buffer, beast::bind_front_handler(&WSSession::on_read, shared_from_this()));
}
