#include "stack.h"

#include <ranges>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Simple") {
    Stack s;
    s.Push(0);
    REQUIRE(s.Top() == 0);
    s.Push(1);
    REQUIRE(s.Top() == 1);
    s.Push(2);
    REQUIRE(s.Top() == 2);

    REQUIRE(s.Pop());
    REQUIRE(s.Top() == 1);
    REQUIRE(s.Pop());
    REQUIRE(s.Top() == 0);
    REQUIRE(s.Pop());
    REQUIRE_FALSE(s.Pop());
}

TEST_CASE("Empty") {
    Stack s;
    REQUIRE(s.Empty());
    REQUIRE(s.Size() == 0);

    s.Push(1);
    REQUIRE_FALSE(s.Empty());
    REQUIRE(s.Size() == 1);
    REQUIRE(s.Pop());
}

TEST_CASE("Long") {
    constexpr auto kRange = std::views::iota(0, 100'000);

    Stack s;
    for (auto i : kRange) {
        REQUIRE(s.Size() == static_cast<size_t>(i));
        s.Push(i);
        REQUIRE(s.Top() == i);
    }
    REQUIRE(s.Size() == kRange.size());

    for (auto i : std::views::reverse(kRange)) {
        REQUIRE(s.Top() == i);
        REQUIRE(s.Pop());
        REQUIRE(s.Size() == static_cast<size_t>(i));
    }
    REQUIRE(s.Empty());
    REQUIRE(s.Size() == 0);
}
