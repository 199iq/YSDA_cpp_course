#pragma once

#include <iterator>

template <class T>
class Vector {
public:
    Vector() {
    }

    Vector(const Vector& other) {
    }
    Vector& operator=(const Vector&) = delete;

    explicit Vector(size_t count, const T& value = {}) {
    }

    template <class Iterator>
    Vector(Iterator first, Iterator last) {
    }

    Vector(std::initializer_list<T> l) {
    }

    Vector Set(size_t index, const T& value) const {
    }

    const T& Get(size_t index) const {
    }

    Vector PushBack(const T& value) const {
    }

    Vector PopBack() const {
    }

    size_t Size() const {
        return 0;
    }
};

template <class Iterator>
Vector(Iterator, Iterator) -> Vector<std::iter_value_t<Iterator>>;
