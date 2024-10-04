#include "itertools.h"

#include <vector>
#include <list>
#include <deque>
#include <forward_list>
#include <initializer_list>
#include <string>
#include <utility>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>

using Catch::Matchers::Equals;

void CheckRepeat(const auto& range, size_t n, const std::vector<int>& expected) {
    std::vector<int> result;
    for (auto x : Repeat(range, n)) {
        result.push_back(x);
    }
    CHECK_THAT(result, Equals(expected));
}

void CheckRepeat(std::initializer_list<int> list, size_t n, const std::vector<int>& expected) {
    CheckRepeat(std::vector(list), n, expected);
    CheckRepeat(std::list(list), n, expected);
    CheckRepeat(std::forward_list(list), n, expected);
}

void CheckRange(int to) {
    auto i = 0;
    for (auto x : Range(to)) {
        if (x != i++) {
            FAIL("Wrong element " << x);
        }
    }
    CHECK(i == to);
}

void CheckRange(int from, int to) {
    auto i = from;
    for (auto x : Range(from, to)) {
        if (x != i++) {
            FAIL("Wrong element " << x);
        }
    }
    CHECK(i == to);
}

void CheckRange(int from, int to, int step) {
    auto i = from;
    for (auto x : Range(from, to, step)) {
        if (x != i) {
            FAIL("Wrong element " << x);
        }
        i += step;
    }
    CHECK(i >= to);
    CHECK(i - step < to);
}

template <class T>
void CheckGroup(const auto& range, const std::vector<std::vector<T>>& expected) {
    std::vector<std::vector<T>> result;
    for (auto group : Group(range)) {
        auto& v = result.emplace_back();
        for (auto x : group) {
            v.push_back(x);
        }
    }
    CHECK_THAT(result, Equals(expected));
}

template <class T>
void CheckGroup(std::initializer_list<T> list, const std::vector<std::vector<T>>& expected) {
    CheckGroup(std::vector(list), expected);
    CheckGroup(std::list(list), expected);
    CheckGroup(std::forward_list(list), expected);
}

TEST_CASE("Repeat zero times") {
    CheckRepeat({}, 0, {});
    CheckRepeat({1}, 0, {});
    CheckRepeat({1, 2}, 0, {});
}

TEST_CASE("Repeat once") {
    CheckRepeat({}, 1, {});
    CheckRepeat({1}, 1, {1});
    CheckRepeat({1, 2}, 1, {1, 2});
    CheckRepeat({3, 2, 1}, 1, {3, 2, 1});
}

TEST_CASE("Repeat many times") {
    CheckRepeat({}, 2, {});
    CheckRepeat({1}, 2, {1, 1});
    CheckRepeat({1, 2}, 2, {1, 2, 1, 2});
    CheckRepeat({}, 3, {});
    CheckRepeat({1, 4, 3}, 3, {1, 4, 3, 1, 4, 3, 1, 4, 3});
}

TEST_CASE("Multi repeat") {
    std::vector v = {2};
    auto r1 = Repeat(v, 2);
    auto r2 = Repeat(r1, 3);
    CheckRepeat(r2, 2, {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2});
}

TEST_CASE("Range [0, to)") {
    CheckRange(5);
    CheckRange(1);
    CheckRange(0);
    CheckRange(1'000'000);
}

TEST_CASE("Range [from, to)") {
    CheckRange(2, 5);
    CheckRange(0, 1);
    CheckRange(10, 10);
    CheckRange(-10'000'000, -9'400'000);
}

TEST_CASE("Range with step") {
    CheckRange(0, 6, 2);
    CheckRange(10, 23, 2);
    CheckRange(-10, 0, 3);
    CheckRange(4, 4, 1);
    CheckRange(4, 10, 20);
    CheckRange(93'234'123, 341'129'431, 1'234);
}

TEST_CASE("Range huge") {
    auto i = 1ll;
    for (auto x : Range(1ll, 1ll << 61)) {
        REQUIRE(x == i++);
        if (i == 100) {
            break;
        }
    }
    REQUIRE(i == 100);
}

TEST_CASE("Zip equal") {
    std::vector v = {1, 3, 5};
    std::list l = {6, 4, 2};

    auto i1 = v.begin();
    auto i2 = l.begin();
    for (const auto& [x, y] : Zip(v, l)) {
        REQUIRE(x == *i1++);
        REQUIRE(y == *i2++);
    }

    REQUIRE(i1 == v.end());
    REQUIRE(i2 == l.end());
}

TEST_CASE("Zip short") {
    std::string s = "abacaba";

    auto it = s.begin();
    int64_t n = 0;
    for (const auto& [c, i] : Zip(s, Range(1ll << 62))) {
        REQUIRE(c == *it++);
        REQUIRE(i == n++);
    }

    REQUIRE(it == s.end());
    REQUIRE(n == std::ssize(s));
}

TEST_CASE("Zip inf") {
    constexpr auto kSize = 100'000;
    auto i = 0ll;
    for (const auto& [i1, i2] : Zip(Range(1ll << 62), Range(1ll << 62))) {
        if (i1 != i) {
            FAIL("Wrong element " << i1);
        } else if (i2 != i) {
            FAIL("Wrong element " << i2);
        } else if (++i == kSize) {
            break;
        }
    }
    REQUIRE(i == kSize);
}

TEST_CASE("Zip empty") {
    std::list<int> l;
    std::deque d = {2.2};

    auto zip = Zip(l, d);
    auto zip2 = Zip(d, l);

    CHECK_FALSE(zip.begin() != zip.end());
    CHECK_FALSE(zip2.begin() != zip2.end());
}

struct Int {
    Int(int x) : x_{x} {
    }

    bool operator==(const Int& rhs) const = default;

private:
    int x_;
};

TEST_CASE("Zip some test") {
    std::deque<Int> d = {5, 4, 3, 1};

    auto i = 0;
    auto it = d.begin();
    for (const auto& [x, y] : Zip(d, Range(3))) {
        REQUIRE(x == *it++);
        REQUIRE(y == i++);
    }

    REQUIRE(i == 3);
    REQUIRE(it == --d.end());
}

TEST_CASE("Group") {
    CheckGroup({2, 2, 2}, {{2, 2, 2}});
    CheckGroup({1}, {{1}});
    CheckGroup({1, 2, 2}, {{1}, {2, 2}});
    CheckGroup({2, 2}, {{2, 2}});
    CheckGroup({1, 1, 2, 2, 2, 3}, {{1, 1}, {2, 2, 2}, {3}});
    CheckGroup<int>({}, {});
    CheckGroup({1, 2, 3}, {{1}, {2}, {3}});
    CheckGroup({4, 4, 2, 2, -3, 4, 4, 4, -1, -1}, {{4, 4}, {2, 2}, {-3}, {4, 4, 4}, {-1, -1}});
}

TEST_CASE("Group some tests") {
    CheckGroup<Int>({2, 2, 2}, {{2, 2, 2}});
    CheckGroup<Int>({1, 2, 2, 1, 3, 3, 3, 2, 2}, {{1}, {2, 2}, {1}, {3, 3, 3}, {2, 2}});
    CheckGroup<Int>({}, {});
}

TEST_CASE("Temporary iterator") {
    std::vector v = {0, 1, 2};

    auto range = Range(5);
    auto zip = Zip(v, range);
    auto [x, y] = *zip.begin();

    REQUIRE(x == y);
}

TEST_CASE("Group and Repeat") {
    std::vector v = {1, 2, 2, 1};

    CheckGroup<int>(Repeat(v, 1), {{1}, {2, 2}, {1}});
    CheckGroup<int>(Repeat(v, 2), {{1}, {2, 2}, {1, 1}, {2, 2}, {1}});
    CheckGroup<int>(Repeat(v, 0), {});
}

TEST_CASE("Group and Range") {
    constexpr auto kSize = 100;

    std::vector<std::vector<int>> expected(kSize);
    for (auto i = 0; auto& v : expected) {
        v.push_back(i++);
    }

    CheckGroup(Range(kSize), expected);
}

TEST_CASE("Group and Zip") {
    std::list l{1, 1, 2, 2, 3, 3, 3};
    std::deque d{1, 1, 1, 2, 2, 3, 3};
    std::vector<std::vector<std::pair<int, int>>> expected = {
        {{1, 1}, {1, 1}}, {{2, 1}}, {{2, 2}}, {{3, 2}}, {{3, 3}, {3, 3}}};
    CheckGroup(Zip(l, d), expected);
}
