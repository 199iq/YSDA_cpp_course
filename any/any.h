#pragma once

#include <type_traits>
#include <concepts>

template <class T>
concept NotAny = !std::same_as<std::remove_cvref_t<T>, class Any>;

class Any {
public:
    Any() {
    }

    // T&& - universal (forwarding) reference
    // use std::forward inside this constructor
    template <NotAny T>
    Any(T&& value) {
    }

    Any(const Any& other) {
    }
    Any(Any&& other) {
    }
    Any& operator=(const Any& other) {
    }
    Any& operator=(Any&& other) {
    }
    ~Any() {
    }

    bool Empty() const {
        return true;
    }

    void Clear() {
    }
    void Swap(Any& other) {
    }

    template <class T>
    const T& GetValue() const {
    }
};
