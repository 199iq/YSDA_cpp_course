#include "ugly.h"

#include <ranges>
#include <stdexcept>
#include <random>
#include <algorithm>
#include <utility>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

TEST_CASE("Spline interpolation works") {
    struct TestCase {
        std::vector<double> x, y;
        std::vector<std::pair<double, double>> checks;
    };

    std::vector<TestCase> test_cases = {
        {{0., 1., 2.}, {0., 0., 0.}, {{0., 0.}, {0.5, 0.}, {1., 0.}, {1.5, 0.}, {2., 0.}}},
        {{0., 1., 2.}, {0., 1., 2.}, {{0., 0.}, {.5, .5}, {1., 1.}, {1.5, 1.5}, {2., 2.}}},
        {{0., 1., 2., 3.},
         {0., 2.71, 3.14, 1.1},
         {{0., 0.},
          {.5, 1.52125},
          {1., 2.71},
          {1.5, 3.28125},
          {2., 3.14},
          {2.5, 2.31},
          {3., 1.1}}}};

    for (auto& test : test_cases) {
        Spline spline{std::move(test.x), std::move(test.y)};
        for (auto [x, y] : test.checks) {
            CHECK(spline.Interpolate(x) == y);
        }
    }
}

TEST_CASE("Fast interpolation") {
    constexpr auto kRange = std::views::iota(0, 1'000'000);

    Spline spline({kRange.begin(), kRange.end()}, {kRange.begin(), kRange.end()});
    for (auto x : kRange) {
        if (auto y = spline.Interpolate(x); y != x) {
            FAIL(x << " != " << y);
        }
    }
}

TEST_CASE("Random cubic") {
    auto f = [](double x) { return .5 * x * x * x - 2.3 * x * x + 3.1 * x - 5.; };
    auto f2 = [](double x) { return 3. * x - 4.6; };

    std::mt19937 gen{5'498'894};
    std::uniform_real_distribution dist{-3., 3.};
    std::vector<std::pair<double, double>> points;
    for (auto i = 0; i < 100; ++i) {
        auto x = dist(gen);
        points.emplace_back(x, f(x));
    }
    std::ranges::sort(points, {}, &std::pair<double, double>::first);

    auto x = std::views::keys(points);
    auto y = std::views::values(points);
    Spline spline{{x.begin(), x.end()}, {y.begin(), y.end()}, f2(x.front()), f2(x.back())};

    for (auto i = 0; i < 10'000; ++i) {
        auto x = dist(gen);
        Catch::Matchers::WithinAbsMatcher matcher{f(x), 1e-12};
        if (auto y = spline.Interpolate(x); !matcher.match(y)) {
            FAIL(y << " != " << f(x));
        }
    }
}

TEST_CASE("Constructor exceptions") {
    std::vector y = {0., 0., 0.};
    CHECK_THROWS_AS((Spline{{1, 2, 2}, y}), std::invalid_argument);
    CHECK_THROWS_AS((Spline{{1, 1, 1}, y}), std::invalid_argument);
    CHECK_THROWS_AS((Spline{{1, 2, 1}, y}), std::invalid_argument);
    CHECK_THROWS_AS((Spline{{2, 1, 2}, y}), std::invalid_argument);
}

TEST_CASE("Interpolate exception") {
    Spline spline{{}, {}};
    CHECK_THROWS_AS(spline.Interpolate(1.3), std::runtime_error);
}
