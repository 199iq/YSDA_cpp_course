#include "matrix.h"

#include <ranges>
#include <cmath>

#include <catch2/catch_test_macros.hpp>

bool EqualMatrix(const Matrix& a, const Matrix& b) {
    if (a.Rows() != b.Rows()) {
        return false;
    }
    if (a.Columns() != b.Columns()) {
        return false;
    }

    for (auto i : std::views::iota(size_t{0}, a.Rows())) {
        for (auto j : std::views::iota(size_t{0}, a.Columns())) {
            if (std::abs(a(i, j) - b(i, j)) > 1e-10) {
                return false;
            }
        }
    }
    return true;
}

TEST_CASE("Constructors") {
    {
        Matrix a(3);
        CHECK(a.Rows() == 3);
        CHECK(a.Columns() == 3);
    }
    {
        Matrix a(3, 5);
        CHECK(a.Rows() == 3);
        CHECK(a.Columns() == 5);
    }
    {
        Matrix a{{{1, 2}, {3, 4}, {5, 6}}};
        CHECK(a.Rows() == 3);
        CHECK(a.Columns() == 2);
        CHECK(a(0, 0) == 1);
        CHECK(a(0, 1) == 2);
        CHECK(a(1, 0) == 3);
        CHECK(a(1, 1) == 4);
        CHECK(a(2, 0) == 5);
        CHECK(a(2, 1) == 6);
    }
}

TEST_CASE("Constness") {
    {
        Matrix a{{{1, 2}, {3, 4}}};
        const auto& b = a;

        CHECK(b.Rows() == 2);
        CHECK(b.Columns() == 2);
        CHECK(b(0, 1) == 2);
    }
    {
        const auto first = Identity(3);
        const Matrix second(3);

        CHECK(EqualMatrix(Transpose(first), first));
        CHECK(EqualMatrix(first - first, second));
        CHECK(EqualMatrix(first * first, first));
        CHECK(EqualMatrix(first + second, first));
    }
}

TEST_CASE("Operations") {
    Matrix a{{{1.0, 2.0, 3.0}, {4.0, 5.0, 6.0}}};
    Matrix b{{{0.0, 1.0, 0.0}, {1.0, 1.0, 2.0}}};
    Matrix c{{{-1.0, -1.0}, {1.0, 1.0}, {1.0, -1.0}}};

    CHECK_FALSE(EqualMatrix(a, Transpose(a)));
    CHECK(EqualMatrix(Transpose(a), {{{1.0, 4.0}, {2.0, 5.0}, {3.0, 6.0}}}));

    auto old_a = a;
    REQUIRE(EqualMatrix(a += b, {{{1.0, 3.0, 3.0}, {5.0, 6.0, 8.0}}}));
    REQUIRE(EqualMatrix(a -= b, old_a));
    REQUIRE(EqualMatrix(a -= a, {2, 3}));

    REQUIRE(EqualMatrix(b * c, {{{1.0, 1.0}, {2.0, -2.0}}}));
    REQUIRE(EqualMatrix(b *= c, {{{1.0, 1.0}, {2.0, -2.0}}}));
}

TEST_CASE("Big") {
    const auto a = Identity(300);
    CHECK(EqualMatrix(a * a, a));
}
