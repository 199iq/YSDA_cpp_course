#pragma once

#include <iostream>
#include <vector>

struct Sum {
    int64_t operator()(int64_t a, int64_t b) {
        return a + b;
    }
};

struct Prod {
    int64_t operator()(int64_t a, int64_t b) {
        return a * b;
    }
};

struct Concat {
    template <class T>
    auto& operator()(std::vector<T>& acc, const std::vector<T>& add) {
        for (auto it = add.begin(); it != add.end(); ++it) {
            acc.emplace_back(*it);
        }
        return acc; // ???
    }
};

class Length {
public:
    explicit Length(size_t* res)
        : res_(res) {
    }

    template <class A, class B>
    auto& operator()(A& a, const B& b) {
        (void) a;
        (void) b;

        (*res_)++;
        return a;
    }
private:
    size_t* res_;
};

template <class Iterator, class T, class BinaryOp>
T Fold(Iterator first, Iterator last, T init, BinaryOp func) {
    T res = init;

    for (auto it = first; it != last; ++it) {
        res = func(res, *it);
    }
    return res;
}
