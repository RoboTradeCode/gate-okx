# okex

Торговый шлюз.

[//]: # (TODO: Добавить описание)

## Установка и сборка

Первое, что вам необходимо сделать — собрать [Boost](https://ru.wikipedia.org/wiki/Boost) 1.78.0, включая компонент
Boost.Log. Вы можете найти подробные инструкции на официальном сайте в
разделе [Getting Started on Unix Variants](https://www.boost.org/doc/libs/1_78_0/more/getting_started/unix-variants.html).

Затем можете клонировать код репозитория:

```shell
git clone --recurse-submodules https://gitlab.com/exchanges2/okx.git
```

> Обратите внимание на параметр `--recurse-submodules`. Он нужен, чтобы рекурсивно установить все зависимости
> репозитория, описанные в файле [.gitmodules](.gitmodules)

Сборка осуществляется с использованием утилиты [CMake](https://ru.wikipedia.org/wiki/CMake). Для её упрощения вы можете
воспользоваться скриптом [build.sh](build.sh). После его исполнения собранный код будет находиться в директории
build/Debug:

```shell
./build.sh
```
