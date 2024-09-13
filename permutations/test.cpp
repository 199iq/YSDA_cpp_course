#include "permutations.h"

#include <vector>
#include <algorithm>
#include <numeric>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>

using Permutations = std::vector<std::vector<int>>;
using Catch::Matchers::Equals;

TEST_CASE("Small") {
    Permutations expected{{0, 1, 2}, {0, 2, 1}, {1, 0, 2}, {1, 2, 0}, {2, 0, 1}, {2, 1, 0}};
    CHECK_THAT(GeneratePermutations(3), Equals(expected));

    expected = {{0}};
    CHECK_THAT(GeneratePermutations(1), Equals(expected));

    expected = {{0, 1}, {1, 0}};
    CHECK_THAT(GeneratePermutations(2), Equals(expected));
}

TEST_CASE("Big") {
    auto size = 8;
    auto permutations = GeneratePermutations(size);
    std::vector<int> expected(size);
    std::iota(expected.begin(), expected.end(), 0);

    size_t i = 0;
    do {
        REQUIRE_THAT(permutations.at(i++), Equals(expected));
    } while (std::ranges::next_permutation(expected).found);
    CHECK(permutations.size() == i);
}
