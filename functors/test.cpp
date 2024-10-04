#include "functors.h"

#include <algorithm>
#include <vector>
#include <ranges>
#include <random>
#include <numeric>
#include <deque>
#include <functional>
#include <string>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>

using Catch::Matchers::Equals;

TEST_CASE("Binary") {
    std::vector a = {1, 3, 4, 0, -1, 6};
    std::ranges::sort(a, MakeReverseBinaryFunctor(std::less_equal{}));
    CHECK_THAT(a, Equals(std::vector{6, 4, 3, 1, 0, -1}));
}

TEST_CASE("String") {
    std::vector<std::string> a = {"aba", "caba"};
    std::ranges::sort(a, MakeReverseBinaryFunctor(std::greater_equal{}));
    std::vector<std::string> expected = {"aba", "caba"};
    CHECK_THAT(a, Equals(expected));

    a = {"aca", "aba", "caba", "abac", "baca"};
    std::ranges::sort(a, MakeReverseBinaryFunctor(std::less_equal{}));
    expected = {"caba", "baca", "aca", "abac", "aba"};
    CHECK_THAT(a, Equals(expected));
}

TEST_CASE("Unary") {
    constexpr auto kIsEven = [](int x) { return x % 2 == 0; };
    constexpr auto kIsPositive = [](int x) { return x > 0; };

    std::vector a = {1, 2, 0, -1, 3, 4};
    std::erase_if(a, MakeReverseUnaryFunctor(kIsEven));
    CHECK_THAT(a, Equals(std::vector{2, 0, 4}));

    a = {-5, 10, 1, 0, -1, 3, -8};
    std::erase_if(a, MakeReverseUnaryFunctor(kIsPositive));
    CHECK_THAT(a, Equals(std::vector{10, 1, 3}));
}

TEST_CASE("ComparisonsCount Int", "[.]") {
    std::vector a = {1, 3, -1, 10, 9, 8, 7, 1, 2};
    CHECK(ComparisonsCount(a.begin(), a.end()) == 31);
}

TEST_CASE("ComparisonsCount Strings", "[.]") {
    std::vector<std::string> a = {"aba", "caba", "aba", "caba", "aba"};
    CHECK(ComparisonsCount(a.begin(), a.end()) == 11);
}

TEST_CASE("ComparisonsCount Big", "[.]") {
    std::deque<int> a(1'000'000);
    std::iota(a.begin(), a.end(), 0);
    std::ranges::shuffle(a, std::mt19937{1'453'123});
    CHECK(ComparisonsCount(a.begin(), a.end()) == 23'558'870);
}
