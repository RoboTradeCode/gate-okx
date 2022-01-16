#include "OKExSession.h"
#include "Publisher.h"
#include "Subscriber.h"

std::atomic<bool> running(true);
std::shared_ptr<Publisher> gateway;
std::shared_ptr<Subscriber> core;
std::shared_ptr<aeron::SleepingIdleStrategy> idle_strategy;

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

    ssl::context ctx{ ssl::context::tls_client };
    ctx.set_verify_mode(ssl::context::verify_peer | ssl::context::verify_fail_if_no_peer_cert);
    ctx.set_default_verify_paths();
    certify::enable_native_https_server_verification(ctx);

    std::make_shared<OKExSession>(ioc, ctx, okex_callback)->run();

    std::thread([&]() {
        ioc.run();
    }).detach();

    gateway = std::make_shared<Publisher>("aeron:ipc", 1001);
    core = std::make_shared<Subscriber>(core_callback, "aeron:ipc", 1002);

    idle_strategy = std::make_shared<aeron::SleepingIdleStrategy>(std::chrono::duration<long, std::milli>(100));

    gateway->connect();
    core->connect();

    signal(SIGINT, sigint_handler);
    while (running)
    {
        idle_strategy->idle(core->poll());
    }

    return EXIT_SUCCESS;
}
