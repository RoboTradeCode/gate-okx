#ifndef AERONEXAMPLES_PUBLISHER_H
#define AERONEXAMPLES_PUBLISHER_H

#include <memory>
#include "Aeron.h"

/**
 * Класс для отправки сообщений с помощью протокола Aeron
 *
 * @param channel Канал Aeron. В общем случае для указания канала используется URI
 * @param stream_id Уникальный идентификатор потока в канале. Значение 0 зарезервировано, его использовать нельзя
 * @param buffer_size Размер буфера, используемого для отправки и приема сообщений
 */
class Publisher
{
private:
    // Медиа-драйвер
    std::shared_ptr<aeron::Aeron> aeron;

    // Канал
    // https://github.com/real-logic/aeron/wiki/Channel-Configuration
    std::string channel;

    // Идентификатор потока
    std::int32_t stream_id;

    // Объект, с помощью которого отправляются сообщения.
    // Содержит в себе информацию о канале, идентификаторе потока и идентификаторе сессии
    std::shared_ptr<aeron::Publication> publication;

    // Буфер, которым инициализируется AtomicBuffer. Напрямую не используется
    std::vector<std::uint8_t> buffer;

    // Буфер, который Aeron использует для отправки сообщения
    // https://github.com/real-logic/aeron/wiki/Cpp-Programming-Guide#atomicbuffer
    aeron::concurrent::AtomicBuffer src_buffer;

public:
    /**
     * @param channel Канал Aeron. В общем случае для указания канала используется URI
     * @param stream_id Уникальный идентификатор потока в канале. Значение 0 зарезервировано, его использовать нельзя
     * @param buffer_size Размер буфера, используемого для отправки и приема сообщений
     */
    explicit Publisher(std::string channel, int32_t stream_id = 1001, int32_t buffer_size = 1400);

    /**
     * Подключение к медиа-драйверу
     *
     * @return Возвращает 0 в случае успеха и -1 в случае ошибки
     */
    int32_t connect();

    /**
     * Отправка сообщения
     *
     * @param message Сообщение для отправки. Может быть фрагментировано в зависимости от размера и MTU
     * @return Новая позиция потока
     */
    std::int64_t offer(const std::string& message);
};

#endif  // AERONEXAMPLES_PUBLISHER_H
