#include "ring_buffer.h"
#include "util.h"

#include <ranges>
#include <random>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Empty") {
    RingBuffer buffer(1);

    REQUIRE(buffer.Size() == 0);
    REQUIRE(buffer.Empty());
}

TEST_CASE("PushAndPop") {
    RingBuffer buffer(2);

    REQUIRE(buffer.TryPush(0));
    REQUIRE(buffer.TryPush(1));
    REQUIRE_FALSE(buffer.TryPush(2));

    REQUIRE(buffer.Size() == 2);
    REQUIRE_FALSE(buffer.Empty());

    int i;
    REQUIRE(buffer.TryPop(&i));
    REQUIRE(i == 0);
    REQUIRE(buffer.TryPop(&i));
    REQUIRE(i == 1);

    REQUIRE_FALSE(buffer.TryPop(&i));
    REQUIRE(buffer.Size() == 0);
    REQUIRE(buffer.Empty());
}

TEST_CASE("Random") {
    std::mt19937 gen{485'392'193};
    std::bernoulli_distribution dist{.6};

    RingBuffer buffer(10);
    auto next_element = 0;
    auto next_received_element = 0;

    for ([[maybe_unused]] auto i : std::views::iota(0, 100'000)) {
        if (dist(gen)) {
            if (buffer.TryPush(next_element)) {
                next_element++;
            }
        } else if (int element; buffer.TryPop(&element)) {
            REQUIRE(next_received_element++ == element);
        }
    }
}

TEST_CASE("Long") {
    constexpr auto kSize = 100'000;
    constexpr auto kRange = std::views::iota(0, kSize);

    RingBuffer buffer(kSize);
    for (auto x : kRange) {
        REQUIRE(buffer.TryPush(x));
    }
    REQUIRE(buffer.Size() == kSize);

    for (auto i : kRange) {
        int x;
        REQUIRE(buffer.TryPop(&x));
        REQUIRE(x == i);
    }
    REQUIRE(buffer.Empty());
    REQUIRE(buffer.Size() == 0);
}

TEST_CASE("LongFull") {
    constexpr auto kSize = 100'000;
    constexpr auto kRange = std::views::iota(0, kSize);

    RingBuffer buffer(kSize);
    for (auto x : kRange) {
        REQUIRE(buffer.TryPush(x));
    }

    for ([[maybe_unused]] auto i : std::views::iota(0, 3 * kSize)) {
        int x;
        REQUIRE(buffer.TryPop(&x));
        REQUIRE(buffer.TryPush(x));
        REQUIRE_FALSE(buffer.TryPush(x));
        REQUIRE(buffer.Size() == kSize);
    }
}

TEST_CASE("Memory") {
    constexpr auto kSize = 5'000'000;

    auto guard = MakeMemoryGuard<int>(3 * kSize);
    RingBuffer buffer(kSize);
    auto ok = true;

    for (auto x : std::views::iota(0, 10 * kSize)) {
        ok &= (buffer.Size() == static_cast<size_t>(std::min(x, kSize)));
        if (buffer.Size() == kSize) {
            int element;
            ok &= buffer.TryPop(&element);
        }
        ok &= buffer.TryPush(x);
    }
    CHECK(ok);
}
