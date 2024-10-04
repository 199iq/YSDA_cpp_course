#include "fold.h"
#include "util.h"
#include "strict_iterator.h"

#include <cstddef>
#include <vector>
#include <list>
#include <ranges>
#include <deque>
#include <utility>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>

using Catch::Matchers::Equals;

auto MakeStrictIterators(const auto& range) {
    auto begin = std::ranges::cbegin(range);
    auto end = std::ranges::cend(range);
    auto first = MakeStrict(begin, begin, end);
    auto last = MakeStrict(begin, end, end);
    return std::make_pair(first, last);
}

void CheckSum(const auto& range, int expected) {
    auto [first, last] = MakeStrictIterators(range);
    auto answer = Fold(first, last, 0, Sum{});
    CHECK(answer == expected);
}

void CheckProd(const auto& range, int expected) {
    auto [first, last] = MakeStrictIterators(range);
    auto answer = Fold(first, last, 1, Prod{});
    CHECK(answer == expected);
}

template <template <class...> class C, class T>
void CheckConcat(const C<std::vector<T>>& v) {
    auto joined = std::views::join(v);
    std::vector expected(joined.begin(), joined.end());
    auto [first, last] = MakeStrictIterators(v);

    auto result = Fold(first, last, std::vector<T>{}, Concat{});
    CHECK_THAT(result, Equals(expected));
}

void CheckLength(const auto& range) {
    using T = std::ranges::range_value_t<decltype(range)>;
    size_t count = 0;
    auto [first, last] = MakeStrictIterators(range);

    Fold(first, last, T{}, Length{&count});
    CHECK(count == range.size());
}

TEST_CASE("Sum") {
    CheckSum(std::vector{1, 2, 3, 6}, 12);
    CheckSum(std::list{-1, 0, 10, 12, 5, -7}, 19);
    CheckSum(std::deque<int>{}, 0);
    CheckSum(std::views::iota(1, 40'001), 800'020'000);
}

TEST_CASE("Prod") {
    CheckProd(std::list{1, 2, -2}, -4);
    CheckProd(std::deque{1, 20, 19, 10, -10}, -38'000);
    CheckProd(std::vector<int>{}, 1);
    CheckProd(std::views::iota(1, 13), 479'001'600);
}

TEST_CASE("Concat") {
    CheckConcat<std::vector, int>({{1, 2}, {3}, {4, 5, 6}, {}});
    CheckConcat<std::vector, double>({{}, {2.2, 1.2}, {}, {0.5}});
    CheckConcat<std::list, int>({{1, 2, 3}, {}, {}, {4, 5, 6}, {}});
    CheckConcat<std::deque, std::string>({{"aba", "caba"}, {"caba"}, {}});
    CheckConcat<std::deque, bool>({{true}, {false}, {true, false}});
}

TEST_CASE("Length") {
    CheckLength(std::list{1, 3, -5, 4});
    CheckLength(std::list<std::string>{"aba", "caba"});
    CheckLength(std::vector{2.2, -0.1, -3.1});
    CheckLength(std::vector<bool>{});
    CheckLength(std::views::iota(500, 1'000'500));
}
