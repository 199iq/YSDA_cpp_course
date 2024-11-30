# Optional

Реализуйте упрощённый аналог шаблона класса
[std::optional<T>](https://en.cppreference.com/w/cpp/utility/optional).
Объект этого класса либо пуст, либо хранит значение типа T.

В этой задаче **не подойдут** следующие реализации:

```cpp
template <class T>
class Optional {
    // ...

private:
    bool has_value_;
    // У T может не быть конструктора по умолчанию
    T value_;
}
```

```cpp
template <class T>
class Optional {
    // ...

private:
    // Мы не хотим делать аллокации
    std::unique_ptr<T> ptr_;
}
```

Предполагается реализация с хранением массива байт подходящего размера и
с подходящим выравниванием для расположения в нём объекта типа `T`.
Для создания объекта в этой памяти используйте
[placement new](https://en.cppreference.com/w/cpp/language/new#Placement_new),
для доступа к объекту в этой памяти используйте `reinterpret_cast`,
а для удаления - явный вызов деструктора.

Также возможна другая реализация через использование union.

## Примечания
* В таком низкоуровневом коде, когда объекты неявно живут в массиве байт,
компилятор не будет автоматически звать деструктор `T` при создании
в буфере нового объекта или при уничтожении самого буфера.
* В некоторых методах используется
[перегрузка по value category](https://en.cppreference.com/w/cpp/language/member_functions#Member_functions_with_ref-qualifier)
* Если ваша реализация `operator=` выглядит примерно так:
```cpp
Optional& operator=(const Optional& other) {
    if (this != &other) {
        Reset();
        // Make copy
        // ...
    }
    return *this;
}
```
то, скорее всего, у вас упадут некоторые тесты. Проблема заключается в том,
что время жизни объекта `other` может зависеть от объекта,
хранящегося в `*this`. Поэтому сначала сделайте копию объекта, 
хранящегося в `other` (если он есть), потом удаляйте текущий объект.
* Запрещено использование
[std::optional](https://en.cppreference.com/w/cpp/utility/optional)
и [std::variant](https://en.cppreference.com/w/cpp/utility/variant).
