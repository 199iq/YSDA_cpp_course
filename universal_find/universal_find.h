#pragma once

#include <algorithm>

template <typename Range, typename T>
auto UniversalFind(Range range, T value) {
    return std::ranges::find(range, value);
}
