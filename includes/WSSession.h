#ifndef OKX_GATEWAY_WSSESSION_H
#define OKX_GATEWAY_WSSESSION_H


#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>

namespace net = boost::asio;
namespace ssl = boost::asio::ssl;
namespace beast = boost::beast;
namespace websocket = beast::websocket;
using tcp = boost::asio::ip::tcp;

class WSSession : public std::enable_shared_from_this<WSSession>
{
    std::shared_ptr<boost::beast::websocket::stream<boost::beast::ssl_stream<boost::beast::tcp_stream>>> ws;
    boost::beast::flat_buffer buffer;
    std::function<void(std::string)> event_handler;

    void on_read(boost::beast::error_code ec, std::size_t bytes_transferred);;

public:
    explicit WSSession(std::string host, const std::string& port, const std::string& target,
        boost::asio::io_context& ioc, std::function<void(std::string)> event_handler);

    void write(const std::string& message);

    void async_read();
};


#endif  // OKX_GATEWAY_WSSESSION_H
