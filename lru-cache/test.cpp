#include "lru_cache.h"
#include "util.h"

#include <string>
#include <ranges>
#include <vector>
#include <algorithm>
#include <concepts>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Check class") {
    STATIC_CHECK_FALSE(std::copy_constructible<LruCache>);
    STATIC_CHECK_FALSE(std::move_constructible<LruCache>);
    STATIC_CHECK_FALSE(std::assignable_from<LruCache, const LruCache&>);
    STATIC_CHECK_FALSE(std::assignable_from<LruCache, LruCache&&>);
}

TEST_CASE("Set and get") {
    LruCache cache(10);
    std::string value;

    cache.Set("a", "1");
    cache.Set("b", "2");
    cache.Set("c", "3");

    REQUIRE(cache.Get("a", &value));
    REQUIRE(value == "1");
    REQUIRE(cache.Get("b", &value));
    REQUIRE(value == "2");
    REQUIRE(cache.Get("c", &value));
    REQUIRE(value == "3");

    REQUIRE(!cache.Get("d", &value));

    cache.Set("c", "4");
    REQUIRE(cache.Get("c", &value));
    REQUIRE(value == "4");
}

TEST_CASE("Eviction") {
    LruCache cache(2);
    std::string value;

    cache.Set("a", "1");
    cache.Set("b", "2");
    cache.Set("c", "3");

    REQUIRE_FALSE(cache.Get("a", &value));
    REQUIRE(cache.Get("b", &value));
    REQUIRE(cache.Get("c", &value));

    cache.Set("b", "4");
    cache.Set("c", "5");
    cache.Set("b", "6");

    cache.Set("e", "7");
    REQUIRE_FALSE(cache.Get("c", &value));
    REQUIRE(cache.Get("b", &value));
    REQUIRE(cache.Get("e", &value));

    cache.Get("b", &value);
    cache.Set("f", "8");
    REQUIRE_FALSE(cache.Get("e", &value));
    REQUIRE(cache.Get("b", &value));
    REQUIRE(cache.Get("f", &value));
}

TEST_CASE("Stress 1") {
    constexpr auto kSize = 1'000;
    constexpr auto kEnd = 100 * kSize;

    LruCache cache(kSize);
    std::string value;
    for (auto i : std::views::iota(0, kSize)) {
        auto key = std::to_string(i);
        cache.Set(key, key);
        if (!cache.Get(key, &value)) {
            FAIL("Can't set " << key);
        } else if (value != key) {
            FAIL(value << " != " << key);
        }
    }

    for (auto i : std::views::iota(kSize, kEnd)) {
        auto key = std::to_string(i);
        cache.Set(key, "foo");
        auto old_key = std::to_string(i - kSize);
        if (cache.Get(old_key, &value)) {
            FAIL(old_key << " was not deleted");
        }
    }
}

TEST_CASE("Stress 2") {
    LruCache cache(100);
    std::string value;
    RandomGenerator rnd{431'234};

    for (auto i = 0; i < 100'000; ++i) {
        auto key = std::to_string(rnd.GenInt<uint32_t>() % 500);
        if (rnd.GenInt<uint32_t>() % 2) {
            cache.Set(key, "foo");
        } else {
            cache.Get(key, &value);
        }
    }
}

TEST_CASE("Stress 3") {
    constexpr auto kSize = 100'000;
    constexpr auto kRange = std::views::iota(0, kSize);

    std::mt19937 gen{31'134};
    std::vector v(kRange.begin(), kRange.end());
    std::ranges::shuffle(v, gen);

    LruCache cache(kSize);
    for (auto i : v) {
        cache.Set(std::to_string(i), "foo");
    }

    std::ranges::shuffle(v, gen);
    std::string value;
    for (auto i : v) {
        auto key = std::to_string(i);
        if (!cache.Get(key, &value)) {
            FAIL(key << " not found");
        }
    }

    for (auto i : std::views::iota(0, kSize)) {
        cache.Set(std::to_string(i + kSize), "foo");
        auto old_key = std::to_string(v[i]);
        if (cache.Get(old_key, &value)) {
            FAIL(old_key << " was not deleted");
        }
    }

    for (auto i : std::views::iota(kSize, 2 * kSize)) {
        auto key = std::to_string(i);
        if (!cache.Get(key, &value)) {
            FAIL(key << " not found");
        }
    }
}
