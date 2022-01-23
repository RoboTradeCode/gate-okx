#include <boost/thread/thread.hpp>
#include <boost/log/trivial.hpp>
#include <boost/json/src.hpp>
#include "includes/OKXPublic.h"
#include "includes/OKXPrivate.h"
#include "includes/Publisher.h"
#include "includes/Subscriber.h"

namespace json = boost::json;

void public_ws_handler(const std::string&);
void private_ws_handler(const std::string&);
void aeron_handler(const std::string&);
void sigint_handler(int);

auto const API_KEY = "37fd8b3f-d35a-4c18-950a-3aa820192344";
auto const PASSPHRASE = "ZVCbDNGr3354";
auto const SECRET_KEY = "8C43BD47C7FF1594D7A33B19F8026A40";

std::atomic<bool> running(true);
std::shared_ptr<OKXPublic> okx_public;
std::shared_ptr<OKXPrivate> okx_private;
std::shared_ptr<Publisher> orderbook_channel;
std::shared_ptr<Publisher> balance_channel;
std::shared_ptr<Publisher> logs_channel;
std::shared_ptr<Subscriber> core_channel;

int main()
{
    // Установка соединения с каналами Aeron
    orderbook_channel = std::make_shared<Publisher>("aeron:ipc", 100);
    balance_channel = std::make_shared<Publisher>("aeron:ipc", 101);
    logs_channel = std::make_shared<Publisher>("aeron:ipc", 102);
    core_channel = std::make_shared<Subscriber>(&aeron_handler, "aeron:ipc", 103);
    BOOST_LOG_TRIVIAL(trace) << "Aeron connections established";

    // Установка соединения с публичным и приватным вебсокетами OKX
    boost::asio::io_context ioc;
    okx_public = std::make_shared<OKXPublic>(ioc, public_ws_handler);
    okx_private = std::make_shared<OKXPrivate>(ioc, private_ws_handler);
    BOOST_LOG_TRIVIAL(trace) << "WebSocket connections established";

    // Авторизация в приватном канале
    okx_private->login(API_KEY, PASSPHRASE, SECRET_KEY);
    BOOST_LOG_TRIVIAL(info) << "Sent request to login";
    boost::this_thread::sleep(boost::posix_time::seconds(3));

    // Подписка на публичный канал тикеров
    // https://www.okx.com/docs-v5/en/#websocket-api-public-channel-tickers-channel
    okx_public->subscribe_tickers("BTC-USDT");
    BOOST_LOG_TRIVIAL(info) << "Sent request to subscribe to tickers channel";

    // Подписка на приватный канал заказов
    // https://www.okx.com/docs-v5/en/#websocket-api-private-channel-order-channel
    okx_private->subscribe_orders();
    BOOST_LOG_TRIVIAL(info) << "Sent request to subscribe to order channel";

    // Подписка на приватный канал баланса и позиций
    // https://www.okx.com/docs-v5/en/#websocket-api-private-channel-balance-and-position-channel
    okx_private->subscribe_balance_and_position();
    BOOST_LOG_TRIVIAL(info) << "Sent request to subscribe to balance and position channel";

    // Опрос вебсокетов и каналов Aeron
    signal(SIGINT, sigint_handler);
    while (running)
    {
        ioc.run_for(std::chrono::milliseconds(100));
        core_channel->poll();
    }

    return EXIT_SUCCESS;
}

void public_ws_handler(const std::string& message)
{
    BOOST_LOG_TRIVIAL(debug) << "Received message in public handler: " << message;
    auto object = json::parse(message).as_object();

    if (!object.if_contains("event"))
    {
        orderbook_channel->offer(json::serialize(json::value{
            { "u", object.at("data").at(0).at("ts") },
            { "s", object.at("arg").at("instId") },
            { "b", object.at("data").at(0).at("bidPx") },
            { "B", object.at("data").at(0).at("bidSz") },
            { "a", object.at("data").at(0).at("askPx") },
            { "A", object.at("data").at(0).at("askSz") },
            { "T", std::to_string(time(nullptr)) }
        }));
    }
    else if (object.at("event") == "error")
    {
        BOOST_LOG_TRIVIAL(error) << object.at("msg");
        logs_channel->offer(json::serialize(json::value{
            { "t", std::to_string(time(nullptr)) },
            { "p", "g" },
            { "n", "OKX" },
            { "c", object.at("code") },
            { "e", object.at("msg") },
        }));
    }
}

void private_ws_handler(const std::string& message)
{
    BOOST_LOG_TRIVIAL(debug) << "Received message in private ws handler: " << message;
    auto object = json::parse(message).as_object();

    if (!object.if_contains("event"))
    {
        balance_channel->offer(json::serialize(json::value{
            // TODO: Proxy balance
        }));
    }
    else if (object.at("event") == "error")
    {
        BOOST_LOG_TRIVIAL(error) << object.at("msg");
        logs_channel->offer(json::serialize(json::value{
            { "t", std::to_string(time(nullptr)) },
            { "p", "g" },
            { "n", "OKX" },
            { "c", object.at("code") },
            { "e", object.at("msg") },
        }));
    }
}

void aeron_handler(const std::string& message)
{
    BOOST_LOG_TRIVIAL(debug) << "Received message in aeron handler: " << message;
    auto object = json::parse(message).as_object();

    if (object.at("a") == "+")
    {
        okx_private->order(
            std::to_string(time(nullptr)),
            object.at("s") == "BUY" ? "buy" : "sell",
            serialize(object.at("S")),
            serialize(object.at("q")),
            serialize(object.at("p"))
        );
        BOOST_LOG_TRIVIAL(info) << "Sent request to create order (id" << std::to_string(time(nullptr)) << ")";
    }
    else
    {
        // TODO: Cancel all orders
        //
        // okx_private->cancel_order(
        //     std::to_string(time(nullptr)),
        //     serialize(object.at("S")),
        //     "<ord_id>"
        // );
        BOOST_LOG_TRIVIAL(info) << "Sent request to cancel order (id" << std::to_string(time(nullptr)) << ")";
    }
}

void sigint_handler(int)
{
    running = false;
}