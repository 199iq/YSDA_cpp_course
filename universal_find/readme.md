# Universal find

У некоторых контейнеров из STL есть метод find, например, `std::unordered_set`. Некотрые контейнеры поддерживают линейный поиск, и для них придется вызывать функцию `std::ranges::find`, например, `std::vector`.

Требуется реализовать шаблонную функцию `UniversalFind`, будет вызывать либо быстрый метод `find`, либо линейный `std::ranges::find`, если метода `find` у контейнера нет.

```c++
std::vector v({3, 4, 5});
auto it = UniversalFind(v, 10); //linear search

std::unordered_set<int> s({3, 4, 5});
...
auto it1 = UniversalFind(s, 42);
```

### Примечания

* Аргументы желательно передавать с помощью perfect forwarding
* https://stackoverflow.com/questions/29182279/how-will-concepts-lite-interact-with-universal-references
* https://en.cppreference.com/w/cpp/language/requires
