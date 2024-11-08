#pragma once

#include <algorithm>
#include <cstddef>

class ListHook {
public:
    bool IsLinked() const;
    void Unlink();

    ListHook(const ListHook&) = delete;
    ListHook& operator=(const ListHook&) = delete;

protected:
    ListHook();

    // Must unlink element from list
    ~ListHook();

    // that helper function might be useful
    void LinkBefore(ListHook* other);

    template <class T>
    friend class List;
};

template <class T>
class List {
public:
    class Iterator {
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using difference_type = ptrdiff_t;
        using pointer = T*;
        using reference = T&;
    };

    List();
    List(const List&) = delete;
    List(List&& other);

    // must unlink all elements from list
    ~List();

    List& operator=(const List&) = delete;
    List& operator=(List&& other);

    bool IsEmpty() const;
    // this method is allowed to be O(n)
    size_t Size() const;

    // note that IntrusiveList doesn't own elements,
    // and never copies or moves T
    void PushBack(T* elem);
    void PushFront(T* elem);

    T& Front();
    const T& Front() const;

    T& Back();
    const T& Back() const;

    void PopBack();
    void PopFront();

    Iterator Begin();
    Iterator End();

    // complexity of this function must be O(1)
    Iterator IteratorTo(T* element);

private:
    ListHook dummy_;
};

template <class T>
List<T>::Iterator begin(List<T>& list) {
    return list.Begin();
}

template <class T>
List<T>::Iterator end(List<T>& list) {
    return list.End();
}
