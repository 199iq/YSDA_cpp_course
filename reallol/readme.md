# Reallol

Это задача типа [crashme](../docs/crash_readme.md).

Исходный код находится в файле `main.cpp`, программа собирается с помощью команды:
```sh
$ g++-12 -std=c++2a -g -fsanitize=address,undefined main.cpp -o run_reallol
```

Вам нужно заполнить файл input.txt, в проверяющей системе он будет использоваться в качестве входа для программы
```sh
$ ./run_reallol < input.txt
```
