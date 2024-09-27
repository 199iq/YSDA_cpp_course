#include "static_map.h"
#include "util.h"

#include <ranges>
#include <string>
#include <random>
#include <vector>
#include <utility>
#include <algorithm>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("EmptyMap") {
    StaticMap map{{}};

    std::string value;
    CHECK_FALSE(map.Find("key", &value));
}

TEST_CASE("SmallMap") {
    StaticMap map{{{"b", "1"}, {"a", "2"}}};

    std::string value;
    REQUIRE(map.Find("b", &value));
    REQUIRE(value == "1");

    REQUIRE(map.Find("a", &value));
    REQUIRE(value == "2");

    CHECK_FALSE(map.Find("c", &value));
    CHECK_FALSE(map.Find("0", &value));
}

TEST_CASE("Random") {
    constexpr auto kRange = std::views::iota(0, 10'000);

    std::mt19937 gen{349'291'934};
    std::bernoulli_distribution dist;
    std::vector<std::pair<std::string, std::string>> items;
    std::vector<bool> contains;
    for (auto i : kRange) {
        if (contains.emplace_back(dist(gen))) {
            items.emplace_back(std::to_string(i), std::to_string(i * i));
        }
    }
    std::ranges::shuffle(items, gen);

    StaticMap map{items};
    std::string value;
    for (auto i : kRange) {
        auto key = std::to_string(i);
        if (contains[i]) {
            REQUIRE(map.Find(key, &value));
            REQUIRE(value == std::to_string(i * i));
        } else {
            REQUIRE_FALSE(map.Find(key, &value));
        }
    }
}

TEST_CASE("Speed") {
    constexpr auto kRange = std::views::iota(0, 100'000);

    std::vector<std::pair<std::string, std::string>> items;
    for (auto i : kRange) {
        items.emplace_back(std::to_string(i), std::to_string(i));
    }
    std::ranges::shuffle(items, std::mt19937{73'467'345});

    StaticMap map{items};
    auto ok = true;
    for (auto i : kRange) {
        std::string value;
        ok &= map.Find(std::to_string(i), &value);
        ok &= (value == std::to_string(i));
    }
    CHECK(ok);
}
