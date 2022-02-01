#include <boost/thread/thread.hpp>
#include <boost/log/trivial.hpp>
#include <boost/json/src.hpp>
#include <boost/algorithm/string.hpp>
#include <algorithm>
#include "OKXPublic.h"
#include "OKXPrivate.h"
#include "OKXREST.h"
#include "Publisher.h"
#include "Subscriber.h"

namespace json = boost::json;

void public_ws_handler(const std::string&);

void private_ws_handler(const std::string&);

void aeron_handler(const std::string&);

void sigint_handler(int);

auto const API_KEY = "b08ae116-38ff-4ab5-8be8-3b7059ff55b1";
auto const PASSPHRASE = "mmkf5jJ5dfC4dgeccnn";
auto const SECRET_KEY = "C132D73EDFA4F73C23C40EF57D0F8086";

std::atomic<bool> running(true);
std::shared_ptr<OKXPublic> okx_public;
std::shared_ptr<OKXPrivate> okx_private;
std::shared_ptr<OKXREST> okx_rest;
std::shared_ptr<Publisher> orderbook_channel;
std::shared_ptr<Publisher> balance_channel;
std::shared_ptr<Publisher> logs_channel;
std::shared_ptr<Subscriber> core_channel;

int main()
{
    // Установка соединения с каналами Aeron
    orderbook_channel = std::make_shared<Publisher>("aeron:ipc", 1100);
    balance_channel = std::make_shared<Publisher>("aeron:ipc", 1101);
    logs_channel = std::make_shared<Publisher>("aeron:ipc", 1102);
    core_channel = std::make_shared<Subscriber>(&aeron_handler, "aeron:ipc", 1103);
    BOOST_LOG_TRIVIAL(trace) << "Aeron connections established";

    // Установка соединений с OKX
    boost::asio::io_context ioc;
    okx_public = std::make_shared<OKXPublic>(ioc, public_ws_handler);
    okx_private = std::make_shared<OKXPrivate>(ioc, private_ws_handler, API_KEY, PASSPHRASE, SECRET_KEY);
    okx_rest = std::make_shared<OKXREST>(ioc, API_KEY, PASSPHRASE, SECRET_KEY);
    BOOST_LOG_TRIVIAL(trace) << "OKX connections established";

    // Авторизация в приватном канале
    okx_private->login();
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

    // Получение списка ордеров
    // https://www.okx.com/docs-v5/en/#rest-api-trade-get-order-list
    BOOST_LOG_TRIVIAL(info) << "Sending request to get order list...";
    auto order_list = json::parse(okx_rest->get_order_list()).as_object();
    BOOST_LOG_TRIVIAL(debug) << "Received order list: " << order_list;

    // Отмена всех ордеров
    for (const auto& order: order_list.at("data").as_array())
    {
        okx_private->cancel_order(
            std::to_string(time(nullptr)),
            std::string(order.at("instId").as_string()),
            std::string(order.at("ordId").as_string())
        );
        BOOST_LOG_TRIVIAL(info) << "Sent request to cancel order (id" << order.at("ordId") << ")";
    }

    // Получение баланса
    // https://www.okx.com/docs-v5/en/#rest-api-account-get-balance
    BOOST_LOG_TRIVIAL(info) << "Sending request to get balance...";
    auto balance = json::parse(okx_rest->get_balance()).as_object();
    BOOST_LOG_TRIVIAL(debug) << "Received balance: " << balance;

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
    BOOST_LOG_TRIVIAL(debug) << "Received message in public ws handler: " << message;
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
    else if (object.if_contains("event") && object.at("event") == "error")
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

    if (!object.if_contains("event") && object.if_contains("arg") &&
        object.at("arg").at("channel") == "balance_and_position")
    {
        json::array balances;
        for (const auto& ccy: object.at("data").at(0).at("balData").as_array())
        {
            balances.push_back(json::value{
                { "a", ccy.at("ccy") },
                { "f", ccy.at("cashBal") },
            });
        }

        if (!balances.empty())
        {
            auto push = json::serialize(json::value{
                { "B", balances }
            });
            balance_channel->offer(push);
            BOOST_LOG_TRIVIAL(info) << "Sent balance to core: " << push;
        }
    }
    else if (object.if_contains("event") && object.at("event") == "error")
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

    if (object.at("a") == "+" && object.at("S") == "BTCUSDT")
    {
        auto id = std::to_string(time(nullptr));
        okx_private->order(
            id,
            object.at("s") == "BUY" ? "buy" : "sell",
            "BTC-USDT",
            std::string(object.at("q").as_string()),
            std::string(object.at("p").as_string())
        );
        BOOST_LOG_TRIVIAL(info) << "Sent request to create order (id" << id << ")";
    }
    else
    {
        std::string side = boost::algorithm::to_lower_copy(std::string(object.at("s").as_string()));

        auto order_list = json::parse(okx_rest->get_order_list()).as_object();
        for (const auto& order: order_list.at("data").as_array())
        {
            if (object.at("S") == "BTCUSDT" && "BTC-USDT" == order.at("instId") && side.c_str() == order.at("side"))
            {
                okx_private->cancel_order(
                    std::to_string(time(nullptr)),
                    std::string(order.at("instId").as_string()),
                    std::string(order.at("ordId").as_string())
                );
                BOOST_LOG_TRIVIAL(info) << "Sent request to cancel order (id" << order.at("ordId") << ")";
            }
        }
    }
}

void sigint_handler(int)
{
    running = false;
}
