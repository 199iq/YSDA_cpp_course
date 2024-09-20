#include "filter_even.h"
#include "util.h"

#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_random.hpp>
#include <catch2/generators/catch_generators_adapters.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>

using Catch::Matchers::Equals;

TEST_CASE("Simple") {
    {
        std::vector test = {1, 4, 3, 2, 6, 7, 0};
        FilterEven(&test);
        CHECK_THAT(test, Equals(std::vector{4, 2, 6, 0}));
    }
    {
        std::vector test = {2, 1, 3, 5};
        FilterEven(&test);
        CHECK_THAT(test, Equals(std::vector{2}));
    }
    {
        std::vector test = {4};
        FilterEven(&test);
        CHECK_THAT(test, Equals(std::vector{4}));
    }
    {
        std::vector test = {1, 6};
        FilterEven(&test);
        CHECK_THAT(test, Equals(std::vector{6}));
    }
    {
        std::vector test = {10, 3};
        FilterEven(&test);
        CHECK_THAT(test, Equals(std::vector{10}));
    }
    {
        std::vector test = {1, 0, 1};
        FilterEven(&test);
        CHECK_THAT(test, Equals(std::vector{0}));
    }
    {
        std::vector test = {7, 3, 5, 4, 6};
        FilterEven(&test);
        CHECK_THAT(test, Equals(std::vector{4, 6}));
    }
}

TEST_CASE("Empty") {
    {
        std::vector<int> test;
        FilterEven(&test);
        CHECK(test.empty());
    }
    {
        std::vector test = {1, -5, -3, 3, 7};
        FilterEven(&test);
        CHECK(test.empty());
    }
}

TEST_CASE("Big") {
    auto seed = GENERATE(take(10, random(0u, 100'000u)));
    auto test = RandomGenerator{seed}.GenIntegralVector(500'000, -1'000'000, 1'000'000);

    auto expected = test;
    std::erase_if(expected, [](auto x) { return x % 2; });
    FilterEven(&test);

    CHECK(test == expected);
}
