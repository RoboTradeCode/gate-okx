# okex

## Установка зависимостей

### Boost

Самый надежный способ получить копию [Boost](https://www.boost.org/) — загрузить дистрибутив с официального сайта. Для
этого перейдите в раздел [«Download»](https://www.boost.org/users/download/) и скопируйте ссылку на скачивание для
платформы `unix`.

На момент написания этого README последней версией boost была 1.78.0. Ссылка на её скачивание выглядит так
— `https://boostorg.jfrog.io/artifactory/main/release/1.78.0/source/boost_1_78_0.tar.bz2`.

Выберите каталог, в который хотите установить Boost, а затем выполните в нём следующую команду, чтобы скачать файл с
помощью [Wget](https://ru.wikipedia.org/wiki/Wget):

```bash
wget https://boostorg.jfrog.io/artifactory/main/release/1.78.0/source/boost_1_78_0.tar.bz2
```

После загрузки разархивируйте Boost:

```bash
tar --bzip2 -xf boost_1_78_0.tar.bz2
```

#### Boost.Certify

Нам также потребуется получить библиотеку [Certify](https://github.com/djarek/certify). Она нужна нам для правильной
проверки подлинности IP-адреса сервера. Её рекомендуют сами разработчики `beast` в комментарии к
файлу [root_certificates.hpp](https://www.boost.org/doc/libs/master/libs/beast/example/common/root_certificates.hpp).

Перейдите в подкаталог, содержащий библиотеки Boost:

```bash
cd boost_1_78_0/libs/
```

Загрузите исходный код:

```bash
git clone https://github.com/djarek/certify.git
```

А затем перейдите обратно в корневую директорию Boost:

```bash
cd ..
```

#### Сборка

Выполните следующую команду для инициализации установщика:

```bash
./bootstrap.sh
```

Затем соберите код и скопируйте библиотеки в систему с помощью:

```bash
sudo ./b2 install
```
