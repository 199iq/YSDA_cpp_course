#include "factorization.h"

#include <vector>
#include <utility>
#include <cstdint>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>

using Catch::Matchers::Equals;
using Factors = std::vector<std::pair<int64_t, int>>;

TEST_CASE("Primes") {
    CHECK_THAT(Factorize(2), Equals(Factors{{2, 1}}));
    CHECK_THAT(Factorize(17), Equals(Factors{{17, 1}}));
    {
        auto num = 1'000'000'007;
        CHECK_THAT(Factorize(num), Equals(Factors{{num, 1}}));
    }
    {
        auto num = 999'999'999'937;
        CHECK_THAT(Factorize(num), Equals(Factors{{num, 1}}));
    }
}

TEST_CASE("Some Tests") {
    CHECK_THAT(Factorize(17'239ll * 17'239), Equals(Factors{{17'239, 2}}));
    CHECK_THAT(Factorize(183'562'547'364), Equals(Factors{{2, 2}, {3, 2}, {7, 2}, {101, 4}}));
    CHECK_THAT(Factorize(185'371'875), Equals(Factors{{3, 3}, {5, 5}, {13, 3}}));
    CHECK_THAT(Factorize(27'984'939'523), Equals(Factors{{23, 4}, {100'003, 1}}));
    CHECK_THAT(Factorize(2 * 2 * 17), Equals(Factors{{2, 2}, {17, 1}}));
}
