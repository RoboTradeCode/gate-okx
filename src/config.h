#ifndef TRADE_CORE_CONFIG_H
#define TRADE_CORE_CONFIG_H


#include <string>
#include <vector>
#include <toml++/toml.h>

// Значения по умолчанию
extern const char* DEFAULT_BTC_THRESHOLD;
extern const char* DEFAULT_USDT_THRESHOLD;
extern const char* DEFAULT_SELL_RATIO;
extern const char* DEFAULT_BUY_RATIO;
extern const char* DEFAULT_LOWER_BOUND_RATIO;
extern const char* DEFAULT_UPPER_BOUND_RATIO;
extern const char* DEFAULT_SUBSCRIBER_CHANNEL;
extern const char* DEFAULT_PUBLISHER_CHANNEL;
extern const int DEFAULT_ORDERBOOKS_STREAM_ID;
extern const int DEFAULT_BALANCE_STREAM_ID;
extern const int DEFAULT_GATEWAY_STREAM_ID;
extern const int DEFAULT_METRICS_STREAM_ID;
extern const int DEFAULT_ERRORS_STREAM_ID;
extern const int DEFAULT_IDLE_STRATEGY_SLEEP_MS;
extern const int DEFAULT_BUFFER_SIZE;

// Конфигурация ядра
struct core_config
{
    struct exchange
    {
        // Пороговые значения для инструментов
        std::string btc_threshold;
        std::string usdt_threshold;

        // Коэффициенты для вычисления цены ордеров
        std::string sell_ratio;
        std::string buy_ratio;

        // Коэффициенты для вычисления границ удержания ордеров
        std::string lower_bound_ratio;
        std::string upper_bound_ratio;
    } exchange;

    struct aeron
    {
        struct subscribers
        {
            // Продолжительность для стратегии ожидания Aeron в мс
            int idle_strategy_sleep_ms{};

            // Subscriber для приёма биржевого стакана
            struct orderbooks
            {
                std::string channel;
                int stream_id;
                std::vector<std::string> destinations;
            } orderbooks;

            // Subscriber для приёма баланса
            struct balance
            {
                std::string channel;
                int stream_id;
                std::vector<std::string> destinations;
            } balance;
        } subscribers;

        struct publishers
        {
            // Publisher для отправки ордеров
            struct gateway
            {
                std::string channel;
                int stream_id;
                int buffer_size;
            } gateway;

            // Publisher для отправки метрик
            struct metrics
            {
                std::string channel;
                int stream_id;
                int buffer_size;
            } metrics;

            // Publisher для отправки ошибок
            struct errors
            {
                std::string channel;
                int stream_id;
                int buffer_size;
            } errors;
        } publishers;
    } aeron;
};

/**
 * Преобразует файл конфигурации в структуру, понятную ядру
 *
 * @param file_path Путь к файлу конфигурации в формате TOML
 * @return Конфигурация ядра
 */
core_config parse_config(std::string_view);


#endif  // TRADE_CORE_CONFIG_H
