#pragma once

#include <cstddef>
#include <iterator>

template <class T>
class List {
public:
    class Iterator {
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        Iterator& operator++();
        Iterator operator++(int);

        Iterator& operator--();
        Iterator operator--(int);

        T& operator*() const;
        T* operator->() const;

        bool operator==(const Iterator&) const;
        bool operator!=(const Iterator&) const;
    };

    List();
    List(const List&);
    List(List&&);
    ~List();

    List& operator=(const List&);
    List& operator=(List&&);

    bool IsEmpty() const;
    size_t Size() const;

    void PushBack(const T&);
    void PushBack(T&&);
    void PushFront(const T&);
    void PushFront(T&&);

    T& Front();
    const T& Front() const;
    T& Back();
    const T& Back() const;

    void PopBack();
    void PopFront();

    void Erase(Iterator);

    Iterator Begin();
    Iterator End();
};

template <class T>
List<T>::Iterator begin(List<T>&);

template <class T>
List<T>::Iterator end(List<T>&);
