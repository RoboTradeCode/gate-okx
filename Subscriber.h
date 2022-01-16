#ifndef AERONEXAMPLES_SUBSCRIBER_H
#define AERONEXAMPLES_SUBSCRIBER_H

#include "Aeron.h"

/**
 * Класс для приёма сообщений с помощью протокола Aeron
 *
 * @param handler Callback для обработки каждого поступающего фрагмента
 * @param channel Канал Aeron. В общем случае для указания канала используется URI
 * @param stream_id Уникальный идентификатор потока в канале. Значение 0 зарезервировано, его использовать нельзя
 */
class Subscriber
{
private:
    // Медиа-драйвер
    std::shared_ptr<aeron::Aeron> aeron;

    // Канал
    // https://github.com/real-logic/aeron/wiki/Channel-Configuration
    std::string channel;

    // Идентификатор потока
    std::int32_t stream_id;

    // Объект, с помощью которого принимаются сообщения.
    // Содержит в себе информацию о канале, идентификаторе потока и идентификаторе сессии
    std::shared_ptr<aeron::Subscription> subscription;

    // Функция обратного вызова для обработки каждого фрагмента сообщения по мере его чтения
    // https://github.com/real-logic/aeron/wiki/Cpp-Programming-Guide#fragment_handler_t
    aeron::fragment_handler_t handler;

public:
    /**
     * @param handler Callback для обработки каждого поступающего фрагмента
     * @param channel Канал Aeron. В общем случае для указания канала используется URI
     * @param stream_id Уникальный идентификатор потока в канале. Значение 0 зарезервировано, его использовать нельзя
     * @param fragments_limit Максимальное количество фрагментов в сообщении
     */
    explicit Subscriber(aeron::fragment_handler_t handler, std::string channel, int32_t stream_id = 1001);

    /**
     * Подключение к медиа-драйверу
     */
    void connect();

    /**
     * Проверка наличия новых сообщений. Если новые сообщения имеются, они будут получены. По мере получения каждое
     * сообщение будет передано в функцию обратного вызова (задаётся в конструкторе класса)
     *
     * @return Количество полученных фрагментов
     */
    int poll();
};

#endif  // AERONEXAMPLES_SUBSCRIBER_H
