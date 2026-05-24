# Homework
### Задание
1. Создайте `CMakeList.txt` для библиотеки *banking*.

See the file [CMakeLists.txt](CMakeLists.txt)

File already includes test building (with a BUILD_TESTS flag)

2. Создайте модульные тесты на классы `Transaction` и `Account`.
    * Используйте mock-объекты.
    * Покрытие кода должно составлять 100%.

See the file [test.cpp](tests/test.cpp)

3. Настройте сборочную процедуру на ~~**TravisCI**~~ **GitHub Actions**.

See the [file](.github/workflows/CI.yml) for GitHub Actions CI

File already has code for Coveralls

4. Настройте [Coveralls.io](https://coveralls.io/).

[![Coverage Status](https://coveralls.io/repos/github/werdfgy/lab05/badge.svg?branch=main)](https://coveralls.io/github/werdfgy/lab05?branch=main)
