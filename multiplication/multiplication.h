#pragma once

#include <stdexcept>

int64_t Multiply(int a, int b) {
    int64_t res = a * 1ll * b;
    // int64_t res = a * b;
    return res;
    // throw std::runtime_error{"Not implemented"};
}
