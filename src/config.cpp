#include "config.h"

// Значения по умолчанию
const char* DEFAULT_BTC_THRESHOLD = "0.0008";
const char* DEFAULT_USDT_THRESHOLD = "40";
const char* DEFAULT_SELL_RATIO = "1.0015";
const char* DEFAULT_BUY_RATIO = "0.9985";
const char* DEFAULT_LOWER_BOUND_RATIO = "0.9995";
const char* DEFAULT_UPPER_BOUND_RATIO = "1.0005";
const char* DEFAULT_SUBSCRIBER_CHANNEL = "aeron:ipc";
const char* DEFAULT_PUBLISHER_CHANNEL = "aeron:ipc?control=localhost:40456|control-mode=dynamic";
const int DEFAULT_ORDERBOOKS_STREAM_ID = 1001;
const int DEFAULT_BALANCE_STREAM_ID = 1002;
const int DEFAULT_GATEWAY_STREAM_ID = 1003;
const int DEFAULT_METRICS_STREAM_ID = 1004;
const int DEFAULT_ERRORS_STREAM_ID = 1005;
const int DEFAULT_IDLE_STRATEGY_SLEEP_MS = 1;
const int DEFAULT_BUFFER_SIZE = 1400;

/**
 * Преобразует файл конфигурации в структуру, понятную ядру
 *
 * @param file_path Путь к файлу конфигурации в формате TOML
 * @return Конфигурация ядра
 */
core_config parse_config(std::string_view file_path)
{
    // Инициализация структуры и корня файла конфигурации
    core_config config;
    toml::table tbl = toml::parse_file(file_path);

    // Сокращения для удобства доступа
    toml::node_view exchange = tbl["exchange"];
    toml::node_view aeron = tbl["aeron"];
    toml::node_view subscribers = aeron["subscribers"];
    toml::node_view publishers = aeron["publishers"];
    toml::node_view balance = subscribers["balance"];
    toml::node_view orderbooks = subscribers["orderbooks"];
    toml::node_view gateway = publishers["gateway"];
    toml::node_view metrics = publishers["metrics"];
    toml::node_view errors = publishers["errors"];

    // Пороговые значения для инструментов
    config.exchange.btc_threshold = exchange["btc_threshold"].value_or(DEFAULT_BTC_THRESHOLD);
    config.exchange.usdt_threshold = exchange["usdt_threshold"].value_or(DEFAULT_USDT_THRESHOLD);

    // Коэффициенты для вычисления цены ордеров
    config.exchange.sell_ratio = exchange["sell_ratio"].value_or(DEFAULT_SELL_RATIO);
    config.exchange.buy_ratio = exchange["buy_ratio"].value_or(DEFAULT_BUY_RATIO);

    // Коэффициенты для вычисления границ удержания ордеров
    config.exchange.lower_bound_ratio = exchange["lower_bound_ratio"].value_or(DEFAULT_LOWER_BOUND_RATIO);
    config.exchange.upper_bound_ratio = exchange["upper_bound_ratio"].value_or(DEFAULT_UPPER_BOUND_RATIO);

    // Продолжительность для стратегии ожидания Aeron в мс
    int idle_strategy_sleep_ms = subscribers["idle_strategy_sleep_ms"].value_or(DEFAULT_IDLE_STRATEGY_SLEEP_MS);
    config.aeron.subscribers.idle_strategy_sleep_ms = idle_strategy_sleep_ms;

    // Subscriber для приёма биржевого стакана
    toml::array* orderbooks_destinations = orderbooks["destinations"].as_array();
    config.aeron.subscribers.orderbooks.channel = orderbooks["channel"].value_or(DEFAULT_SUBSCRIBER_CHANNEL);
    config.aeron.subscribers.orderbooks.stream_id = orderbooks["stream_id"].value_or(DEFAULT_ORDERBOOKS_STREAM_ID);
    for (toml::node& destination: *orderbooks_destinations)
        config.aeron.subscribers.orderbooks.destinations.emplace_back(destination.value_or(""));

    // Subscriber для приёма баланса
    toml::array* balance_destinations = balance["destinations"].as_array();
    config.aeron.subscribers.balance.channel = balance["channel"].value_or(DEFAULT_SUBSCRIBER_CHANNEL);
    config.aeron.subscribers.balance.stream_id = balance["stream_id"].value_or(DEFAULT_BALANCE_STREAM_ID);
    for (toml::node& destination: *balance_destinations)
        config.aeron.subscribers.balance.destinations.emplace_back(destination.value_or(""));

    // Publisher для отправки ордеров
    config.aeron.publishers.gateway.channel = gateway["channel"].value_or(DEFAULT_PUBLISHER_CHANNEL);
    config.aeron.publishers.gateway.stream_id = gateway["stream_id"].value_or(DEFAULT_GATEWAY_STREAM_ID);
    config.aeron.publishers.gateway.buffer_size = gateway["buffer_size"].value_or(DEFAULT_BUFFER_SIZE);

    // Publisher для отправки метрик
    config.aeron.publishers.metrics.channel = metrics["channel"].value_or(DEFAULT_PUBLISHER_CHANNEL);
    config.aeron.publishers.metrics.stream_id = metrics["stream_id"].value_or(DEFAULT_METRICS_STREAM_ID);
    config.aeron.publishers.metrics.buffer_size = metrics["buffer_size"].value_or(DEFAULT_BUFFER_SIZE);

    // Publisher для отправки ошибок
    config.aeron.publishers.errors.channel = errors["channel"].value_or(DEFAULT_PUBLISHER_CHANNEL);
    config.aeron.publishers.errors.stream_id = errors["stream_id"].value_or(DEFAULT_ERRORS_STREAM_ID);
    config.aeron.publishers.errors.buffer_size = errors["buffer_size"].value_or(DEFAULT_BUFFER_SIZE);

    return config;
}
