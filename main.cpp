#include "OKExSession.h"
#include "Publisher.h"
#include "Subscriber.h"

std::atomic<bool> running(true);
std::shared_ptr<Publisher> gateway;
std::shared_ptr<Subscriber> core;

void okex_callback(const std::string& message)
{
    std::cout << message << std::endl;
    gateway->offer(message);
}

void core_callback(
        const aeron::AtomicBuffer& buffer,
        aeron::util::index_t offset,
        aeron::util::index_t length,
        const aeron::Header& header)
{
    const char* s = reinterpret_cast<const char*>(buffer.buffer()) + offset;
    auto n = static_cast<std::size_t>(length);
    std::string message(s, n);
    std::cout << message << std::endl;
}

void sigint_handler(int)
{
    running = false;
}

int main()
{
    net::io_context ioc;
    std::shared_ptr<OKExSession> okex = std::make_shared<OKExSession>(ioc, okex_callback);

    gateway = std::make_shared<Publisher>("aeron:ipc", 1001);
    core = std::make_shared<Subscriber>(core_callback, "aeron:ipc", 1002);

    gateway->connect();
    core->connect();

    json::value jv = {
        { "op", "subscribe" },
        { "args", {
            {
                { "channel", "tickers" },
                { "instId", "BTC-USDT" },
            }
        }}
    };

    okex->write(json::serialize(jv));
    okex->async_read();

    signal(SIGINT, sigint_handler);
    while (running)
    {
        ioc.run_for(std::chrono::milliseconds(100));
        core->poll();
    }

    return EXIT_SUCCESS;
}
