#include "includes/HTTPSession.h"

HTTPSession::HTTPSession(const std::string& host, const std::string& port, net::io_context& ioc)
    : host(host)
{
    ssl::context ctx{ ssl::context::tls_client };
    ctx.set_verify_mode(ssl::context::verify_peer | ssl::context::verify_fail_if_no_peer_cert);
    ctx.set_default_verify_paths();
    boost::certify::enable_native_https_server_verification(ctx);

    stream = std::make_shared<beast::ssl_stream<beast::tcp_stream>>(ioc, ctx);

    tcp::resolver resolver{ ioc };
    auto results = resolver.resolve(host, port);
    beast::get_lowest_layer(*stream).connect(results);

    SSL_set_tlsext_host_name(stream->native_handle(), host.c_str());
    stream->handshake(ssl::stream_base::client);
}

std::string HTTPSession::get(const std::string& target, const std::string& api_key, const std::string& passphrase,
    const std::string& secret_key)
{
    auto timestamp = to_iso_extended_string(boost::posix_time::microsec_clock::universal_time()) + "Z";
    auto sign = base64_hmac_sha256(timestamp + "GET" + "/users/self/verify", secret_key);

    http::request<http::string_body> req{ http::verb::get, target, 11 };
    req.set(http::field::host, host);
    req.set("x-simulated-trading", "1");
    req.set("OK-ACCESS-KEY", api_key);
    req.set("OK-ACCESS-SIGN", sign);
    req.set("OK-ACCESS-TIMESTAMP", timestamp);
    req.set("OK-ACCESS-PASSPHRASE", passphrase);

    http::write(stream, req);

    beast::flat_buffer buffer;
    http::response<http::string_body> res;
    http::read(stream, buffer, res);

    return beast::buffers_to_string(res);
}
