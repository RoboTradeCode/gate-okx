#include "Publisher.h"

/**
 * @param channel Канал Aeron. В общем случае для указания канала используется URI
 * @param stream_id Уникальный идентификатор потока в канале. Значение 0 зарезервировано, его использовать нельзя
 * @param buffer_size Размер буфера, используемого для отправки и приема сообщений
 */
Publisher::Publisher(std::string channel, int32_t stream_id, int32_t buffer_size) :
        channel(std::move(channel)),
        stream_id(stream_id),
        buffer(buffer_size, 0),
        src_buffer(&buffer[0], buffer.size())
{
}

/**
 * Подключение к медиа-драйверу
 *
 * @return Возвращает 0 в случае успеха и -1 в случае ошибки
 */
int32_t Publisher::connect()
{
    try
    {
        // Подключение к медиа-драйверу
        // https://github.com/real-logic/aeron/wiki/Cpp-Programming-Guide#aeron
        aeron = aeron::Aeron::connect();

        // Инициализация объекта Publication, с помощью которого отправляются сообщения
        //
        // Сначала вызывается метод addPublication. Этот метод неблокирующий — сначала он возвращает результат (уникальный
        // идентификатор регистрации) и только потом пытается добавить Publication. Чтобы убедиться в том, что операция
        // добавления прошла успешно и получить объект Publication, нужно дополнитель вызвать метод findPublication, передав
        // ему идентификатор регистрации
        //
        // Метод findPublication тоже является неблокирующим и сразу же возвращает результат. Но операция добавления
        // Publication занимает некоторое время. И если вызвать findPublication в момент, когда Publication ещё не
        // добавлен, вернётся nullptr. Поэтому findPublication вызывается в цикле — до тех пор, пока вместо nullptr не
        // вернётся объект
        //
        // https://github.com/real-logic/aeron/wiki/Cpp-Programming-Guide#publication
        std::int64_t id = aeron->addPublication(channel, stream_id);
        publication = aeron->findPublication(id);
        while (!publication)
        {
            std::this_thread::yield();
            publication = aeron->findPublication(id);
        }
    }
    catch (...) {
        return -1;
    }

    return 0;
}

/**
 * Отправка сообщения
 *
 * @param message Сообщение для отправки. Может быть фрагментировано в зависимости от размера и MTU
 * @return Новая позиция потока
 */
std::int64_t Publisher::offer(const std::string& message)
{
    // Отправка данных
    //
    // При вызове offer возвращаемое значение, превышающее 0, означает, что сообщение было отправлено.
    // Отрицательные значения означают, что произошла ошибка
    //
    // https://github.com/real-logic/aeron/wiki/Cpp-Programming-Guide#handling-back-pressure
    src_buffer.putStringWithoutLength(0, message);
    const std::int64_t result = publication->offer(src_buffer, 0, message.length());
    return result;
}
