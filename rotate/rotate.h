#pragma once

#include <stdexcept>
#include <vector>
#include <cstddef>

void Reverse(std::vector<int>* data, size_t left, size_t right) {
    std::vector<int>& vec = *data;

    size_t mid = (left + right + 1) / 2;
    for (size_t i = left; i < mid; i++) {
        size_t next = right - (i - left);

        std::swap(vec[i], vec[next]);
    }
}

void Rotate(std::vector<int>* data, size_t shift) {
    std::vector<int>& vec = *data;

    size_t n = vec.size();
    if (!n || !shift) {
        return;
    }

    Reverse(data, 0, shift - 1);
    Reverse(data, shift, n - 1);
    Reverse(data, 0, n - 1);
}
