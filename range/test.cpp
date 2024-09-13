#include "range.h"

#include <vector>
#include <ranges>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>

using Catch::Matchers::Equals;
using Catch::Matchers::RangeEquals;

TEST_CASE("Simple") {
    CHECK_THAT(Range(2, 5, 1), Equals(std::vector{2, 3, 4}));
    CHECK_THAT(Range(1, 5, 2), Equals(std::vector{1, 3}));
    CHECK_THAT(Range(-9, 10, 5), Equals(std::vector{-9, -4, 1, 6}));
}

TEST_CASE("SimpleReverse") {
    CHECK_THAT(Range(5, 2, -1), Equals(std::vector{5, 4, 3}));
    CHECK_THAT(Range(5, 1, -2), Equals(std::vector{5, 3}));
    CHECK_THAT(Range(7, 6, -3), Equals(std::vector{7}));
}

TEST_CASE("Empty") {
    CHECK(Range(0, 0, 2).empty());
    CHECK(Range(2, 2, 1).empty());
    CHECK(Range(10, 10, -2).empty());
}

TEST_CASE("EmptyInf") {
    CHECK(Range(3, 7, -1).empty());
    CHECK(Range(3, 4, 0).empty());
    CHECK(Range(5, -5, 2).empty());
    CHECK(Range(3, -7, 0).empty());
}

TEST_CASE("Big") {
    auto from = -1'000'000;
    auto to = 1'000'000;
    auto range = std::views::iota(from, to);
    CHECK_THAT(Range(from, to), RangeEquals(range));
    CHECK_THAT(Range(to - 1, from - 1, -1), RangeEquals(range | std::views::reverse));
}
