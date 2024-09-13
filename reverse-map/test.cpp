#include "reverse_map.h"

#include <string>
#include <unordered_map>
#include <ranges>

#include <catch2/catch_test_macros.hpp>

using Map = std::unordered_map<std::string, int>;
using ReversedMap = std::unordered_map<int, std::string>;

TEST_CASE("Simple") {
    {
        Map map{{"aba", 3}, {"caba", 1}, {"test", 2}};
        ReversedMap expected{{1, "caba"}, {2, "test"}, {3, "aba"}};
        CHECK(ReverseMap(map) == expected);
    }
    {
        Map map{{"", 0}, {"1", 1}};
        ReversedMap expected{{0, ""}, {1, "1"}};
        CHECK(ReverseMap(map) == expected);
    }
    {
        Map map{{"hello", -10}, {"world", 3}};
        ReversedMap expected{{-10, "hello"}, {3, "world"}};
        CHECK(ReverseMap(map) == expected);
    }
}

TEST_CASE("Empty") {
    CHECK(ReverseMap({}).empty());
}

TEST_CASE("Big") {
    Map map;
    ReversedMap expected;
    for (auto i : std::views::iota(0, 100'000)) {
        map.emplace(std::to_string(i), i);
        expected.emplace(i, std::to_string(i));
    }
    CHECK(ReverseMap(map) == expected);
}
