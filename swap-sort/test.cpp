#include "swap_sort.h"

#include <limits>
#include <array>
#include <algorithm>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Swap test") {
    {
        auto a = 5, b = 7;
        Swap(&a, &b);
        CHECK(a == 7);
        CHECK(b == 5);
    }
    {
        auto a = std::numeric_limits<int>::min();
        auto b = std::numeric_limits<int>::max();
        Swap(&a, &b);
        CHECK(a == std::numeric_limits<int>::max());
        CHECK(b == std::numeric_limits<int>::min());
    }
    {
        auto a = 1, b = 1;
        Swap(&a, &b);
        CHECK(a == 1);
        CHECK(b == 1);
    }
}

TEST_CASE("Sort All") {
    std::array p{1, 2, 3};
    do {
        auto [a, b, c] = p;
        Sort3(&a, &b, &c);
        CHECK(a == 1);
        CHECK(b == 2);
        CHECK(c == 3);
    } while (std::ranges::next_permutation(p).found);
}

TEST_CASE("Sort Eq") {
    {
        auto a = 1, b = 1, c = 1;
        Sort3(&a, &b, &c);
        CHECK(a == 1);
        CHECK(b == 1);
        CHECK(c == 1);
    }
    {
        auto a = 2, b = 4, c = 2;
        Sort3(&a, &b, &c);
        CHECK(a == 2);
        CHECK(b == 2);
        CHECK(c == 4);
    }
    {
        auto a = 3, b = 3, c = 1;
        Sort3(&a, &b, &c);
        CHECK(a == 1);
        CHECK(b == 3);
        CHECK(c == 3);
    }
}
