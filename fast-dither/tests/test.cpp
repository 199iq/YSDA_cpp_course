#include "../kd_tree.h"
#include "../dither.h"
#include "util.h"
#include "image.h"

#include <vector>
#include <cmath>
#include <utility>
#include <algorithm>
#include <filesystem>
#include <ranges>

#include <catch2/catch_test_macros.hpp>

namespace {

const auto kDir = GetFileDir(__FILE__);

bool IsEqual(const RGB& lhs, const RGB& rhs) {
    return (lhs.r == rhs.r) && (lhs.g == rhs.g) && (lhs.b == rhs.b);
};

void CheckPixels(const Image& image, const Image& expected) {
    auto height = image.Height();
    auto width = image.Width();
    REQUIRE(height == expected.Height());
    REQUIRE(width == expected.Width());
    auto count = 0;
    for (auto y : std::views::iota(0, height)) {
        for (auto x : std::views::iota(0, width)) {
            count += IsEqual(image.GetPixel(y, x), expected.GetPixel(y, x));
        }
    }
    CHECK(count >= .99 * height * width);
}

std::vector<RGB> ReadColors(const std::filesystem::path& path) {
    std::ifstream in{path};
    std::vector<RGB> colors;
    RGB color;
    while (in >> color.r >> color.g >> color.b) {
        colors.push_back(color);
    }
    return colors;
}

}  // namespace

TEST_CASE("Read image") {
    Image image{kDir / "2.png"};
    REQUIRE(image.Height() == 512);
    REQUIRE(image.Width() == 512);
    CHECK(IsEqual(image.GetPixel(2, 2), {223, 137, 133}));
    CHECK(IsEqual(image.GetPixel(0, 256), {208, 109, 108}));
    CHECK(IsEqual(image.GetPixel(1, 511), {200, 99, 90}));
}

TEST_CASE("1-d tree") {
    KdTree<1> tree{{{0}, {1}, {2}, {3}, {4}}};
    for (auto i : std::views::iota(size_t{0}, size_t{5})) {
        CHECK(tree.GetNearest({i + .25}) == i);
    }
}

TEST_CASE("Simple 3-d tree") {
    KdTree<3> tree{{{0, 0, 0}, {1, 2, 3}, {1, 1, 0}}};
    CHECK(tree.GetNearest({0, 0, 0}) == 0);
    CHECK(tree.GetNearest({1, 2, 3}) == 1);
    CHECK(tree.GetNearest({1, 1, 0}) == 2);
    CHECK(tree.GetNearest({1, 1, 1}) == 2);
}

TEST_CASE("3-d tree") {
    constexpr auto kCount = 10'000;
    constexpr auto kDoubleToPoint = [](double x) { return Point<3>{x, x, 0.}; };

    auto range = std::views::iota(0, kCount) | std::views::transform(kDoubleToPoint);
    KdTree<3> tree{{range.begin(), range.end()}};
    RandomGenerator gen{47'875'656};

    for (auto i = 0; i < 30'000; ++i) {
        auto point = gen.GenRealArray<3>(0, kCount);
        auto x = (point[0] + point[1]) / 2.;
        auto expected = static_cast<int>(std::lround(x));
        expected = std::min(expected, kCount - 1);
        auto index = static_cast<int>(tree.GetNearest(point));
        if (index != expected) {
            FAIL_CHECK(index << " != " << expected);
        }
    }
}

TEST_CASE("512-d tree") {
    constexpr auto kSize = 10'000;
    RandomGenerator gen{58'141};

    Points<512> points(kSize);
    for (auto& point : points) {
        point = gen.GenRealArray<512>(-100, 100);
    }

    KdTree tree{points};
    for (auto i : std::views::iota(0, kSize)) {
        auto answer = tree.GetNearest(points[i]);
        if (answer != static_cast<size_t>(i)) {
            FAIL_CHECK(answer << " != " << i);
        }
    }
}

TEST_CASE("2 colors") {
    auto image = Dither(Image{kDir / "2.png"}, {{0, 0, 0}, {255, 255, 255}});
    CheckPixels(image, Image{kDir / "2_result.png"});
}

TEST_CASE("16 colors") {
    auto colors = ReadColors(kDir / "16_colors.txt");
    REQUIRE(colors.size() == 16);
    auto image = Dither(Image{kDir / "16.png"}, colors);
    CheckPixels(image, Image{kDir / "16_result.png"});
}

TEST_CASE("128 colors") {
    auto colors = ReadColors(kDir / "128_colors.txt");
    REQUIRE(colors.size() == 128);
    auto image = Dither(Image{kDir / "128.png"}, colors);
    CheckPixels(image, Image{kDir / "128_result.png"});
}
