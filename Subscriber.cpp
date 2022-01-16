#include "Subscriber.h"
#include <utility>

/**
 * @param handler Callback для обработки каждого поступающего фрагмента
 * @param channel Канал Aeron. В общем случае для указания канала используется URI
 * @param stream_id Уникальный идентификатор потока в канале. Значение 0 зарезервировано, его использовать нельзя
 */
Subscriber::Subscriber(aeron::fragment_handler_t handler, std::string channel, int32_t stream_id) :
        handler(std::move(handler)),
        channel(std::move(channel)),
        stream_id(stream_id)
{
}

/**
 * Подключение к медиа-драйверу
 */
void Subscriber::connect()
{
    // Подключение к медиа-драйверу
    // https://github.com/real-logic/aeron/wiki/Cpp-Programming-Guide#aeron
    aeron = aeron::Aeron::connect();

    // Сначала вызывается метод addSubscription. Этот метод неблокирующий — сначала он возвращает результат (уникальный
    // идентификатор регистрации) и только потом пытается добавить Subscription. Чтобы убедиться в том, что операция
    // добавления прошла успешно и получить объект Subscription, нужно дополнитель вызвать метод findSubscription,
    // передав ему идентификатор регистрации
    //
    // Метод findSubscription тоже является неблокирующим и сразу же возвращает результат. Но операция добавления
    // Subscription занимает некоторое время. И если вызвать findSubscription в момент, когда Subscription ещё не
    // добавлен, вернётся nullptr. Поэтому findSubscription вызывается в цикле — до тех пор, пока вместо nullptr не
    // вернётся объект
    //
    // https://github.com/real-logic/aeron/wiki/Cpp-Programming-Guide#subscription
    std::int64_t id = aeron->addSubscription(channel, stream_id);
    subscription = aeron->findSubscription(id);
    while (!subscription)
    {
        std::this_thread::yield();
        subscription = aeron->findSubscription(id);
    }
}

/**
 * Проверка наличия новых сообщений. Если новые сообщения имеются, они будут получены. По мере получения каждое
 * сообщение будет передано в функцию обратного вызова (задаётся в конструкторе класса)
 *
 * @return Количество полученных фрагментов
 */
int Subscriber::poll()
{
    // Определяет, есть ли какие-либо фрагменты для получения и получает их
    // https://github.com/real-logic/aeron/wiki/Cpp-Programming-Guide#polling
    return subscription->poll(handler, 1);
}
