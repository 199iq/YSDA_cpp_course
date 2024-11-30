#pragma once

template <class T>
class Optional {
public:
    Optional() {
    }

    Optional(const T& value) {
    }

    Optional(T&& value) {
    }

    Optional(const Optional& other) {
    }

    // This constructor must not change other.HasValue()
    // Just move value (if present)
    Optional(Optional&& other) {
    }

    ~Optional() {
    }

    Optional& operator=(const Optional& other) {
    }

    // This method must not change other.HasValue()
    // Just move value (if present)
    Optional& operator=(Optional&& other) {
    }

    T& operator*() & {
    }

    const T& operator*() const& {
    }

    T&& operator*() && {
    }

    T* operator->() {
    }

    const T* operator->() const {
    }

    T& Value() & {
    }

    const T& Value() const& {
    }

    T&& Value() && {
    }

    bool HasValue() const {
    }

    void Reset() {
    }
};
