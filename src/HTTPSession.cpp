#include "includes/HTTPSession.h"

HTTPSession::HTTPSession(const std::string& host, const std::string& port, net::io_context& ioc)
    : host(host)
{
    ssl::context ctx{ ssl::context::tls_client };
    ctx.set_verify_mode(ssl::context::verify_peer | ssl::context::verify_fail_if_no_peer_cert);
    ctx.set_default_verify_paths();

    s = std::make_shared<beast::ssl_stream<beast::tcp_stream>>(ioc, ctx);

    tcp::resolver resolver(ioc);
    auto const results = resolver.resolve(host, port);
    beast::get_lowest_layer(*s).connect(results);

    SSL_set_tlsext_host_name(s->native_handle(), host.c_str());
    s->handshake(ssl::stream_base::client);
}

std::string HTTPSession::get(const std::string& target, const std::string& api_key, const std::string& passphrase,
    const std::string& secret_key)
{
    auto microsec_time = to_iso_extended_string(boost::posix_time::microsec_clock::universal_time());
    auto timestamp = microsec_time.substr(0, microsec_time.size() - 3) + "Z";
    auto sign = base64_hmac_sha256(timestamp + "GET" + target, secret_key);

    http::request<http::string_body> req{ http::verb::get, target, 11 };
    req.set(http::field::host, host);

    req.set("OK-ACCESS-KEY", api_key);
    req.set("OK-ACCESS-SIGN", sign);
    req.set("OK-ACCESS-TIMESTAMP", timestamp);
    req.set("OK-ACCESS-PASSPHRASE", passphrase);

    http::write(*s, req);

    beast::flat_buffer buffer;
    http::response<http::string_body> res;
    http::read(*s, buffer, res);

    return res.body();
}
