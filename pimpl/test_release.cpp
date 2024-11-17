#include "ugly.h"
#include "util.h"

#include <ranges>
#include <stdexcept>
#include <vector>
#include <cstddef>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

void TestMemoryException(double coef) {
    constexpr auto kRange = std::views::iota(0, 10'000'000);

    std::vector<double> x(kRange.begin(), kRange.end());
    auto y = x;

    auto limit = static_cast<size_t>(coef * kRange.size());
    auto guard = MakeMemoryGuard<double>(limit);
    if (coef < 2.) {
        CHECK_THROWS_AS((Spline{std::move(x), std::move(y)}), std::bad_alloc);
    } else {
        CHECK_NOTHROW(Spline{std::move(x), std::move(y)});
    }
}

TEST_CASE("Memory exceptions") {
    TestMemoryException(.5);
    TestMemoryException(1.5);

    TestMemoryException(4.5);
    TestMemoryException(3.5);
    TestMemoryException(2.5);
}

TEST_CASE("No extra memory in Interpolate") {
    auto range = std::views::iota(0, 10'000'000);
    std::vector<double> x(range.begin(), range.end());
    auto y = x;
    Spline spline{std::move(x), std::move(y)};

    auto guard = MakeMemoryGuard<std::byte>(100'000);
    for (auto x : range) {
        if (auto y = spline.Interpolate(x); y != x) {
            FAIL(y << " != " << x);
        }
    }
}
