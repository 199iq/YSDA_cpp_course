#include "universal_find.h"

#include <vector>
#include <deque>
#include <string>
#include <utility>
#include <set>
#include <algorithm>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Move only elements") {
    struct MoveOnly {
        explicit MoveOnly(int x) : x_(x) {
        }
        MoveOnly(const MoveOnly&) = delete;
        MoveOnly& operator=(const MoveOnly&) = delete;

        MoveOnly(MoveOnly&&) noexcept = default;
        MoveOnly& operator=(MoveOnly&&) noexcept = default;

        auto operator<=>(const MoveOnly& other) const = default;

        int x_{0};
    };
    std::vector<MoveOnly> v;
    for (int i = 0; i < 10; ++i) {
        v.emplace_back(i);
    }

    CHECK(UniversalFind(v, MoveOnly(5)) == std::ranges::find(v, MoveOnly(5)));
    CHECK(UniversalFind(v, MoveOnly(10)) == std::ranges::find(v, MoveOnly(10)));
}

TEST_CASE("NonCopyable") {
    struct NonCopyable {
        explicit NonCopyable(int x) : x_(x) {
        }
        NonCopyable(const NonCopyable&) = delete;
        NonCopyable& operator=(const NonCopyable&) = delete;

        auto operator<=>(const NonCopyable& other) const = default;

        int x_{0};
    };

    std::deque<NonCopyable> v;
    for (int i = 0; i < 10; ++i) {
        v.emplace_back(i);
    }
    CHECK(UniversalFind(v, NonCopyable(5)) == std::ranges::find(v, NonCopyable(5)));
    CHECK(UniversalFind(v, NonCopyable(10)) == std::ranges::find(v, NonCopyable(10)));
}

template <typename T>
class Set : public std::set<T> {
    using Base = std::set<T>;

public:
    explicit Set(auto&&... args) : Base(std::forward<decltype(args)>(args)...) {
    }

    // NOLINTNEXTLINE
    auto find(auto&& key) const {
        ++cnt_;
        return Base::find(std::forward<decltype(key)>(key));
    }

    size_t GetCnt() const noexcept {
        return cnt_;
    }

private:
    mutable size_t cnt_{0};
};

TEST_CASE("Fast find") {
    auto init = {1, 2, 3, 4, 5};
    Set<int> s(std::move(init));

    CHECK(UniversalFind(s, 5) == std::ranges::find(s, 5));
    CHECK(UniversalFind(s, 10) == std::ranges::find(s, 10));
    CHECK(s.GetCnt() == 2);
}
