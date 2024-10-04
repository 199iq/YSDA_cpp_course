#include "find_last.h"
#include "util.h"
#include "strict_iterator.h"

#include <string>
#include <vector>
#include <unordered_map>
#include <list>
#include <iterator>
#include <ranges>

#include <catch2/catch_test_macros.hpp>

template <class Iterator>
void Check(Iterator first, Iterator last, const std::iter_value_t<Iterator>& value,
           Iterator expected) {
    auto first_strict = MakeStrict(first, first, last);
    auto last_strict = MakeStrict(first, last, last);
    auto it = FindLast(first_strict, last_strict, value).Base();
    CHECK(it == expected);
    if (expected != last) {
        CHECK(*it == value);
    }
}

TEST_CASE("Simple") {
    std::vector<std::string> data = {"aba", "caba", "aba"};
    CHECK(FindLast(data.begin(), data.end(), "caba") == data.begin() + 1);
    CHECK(FindLast(data.begin(), data.end(), "ada") == data.end());
    Check(data.begin(), data.end(), "aba", data.begin() + 2);
}

TEST_CASE("Empty") {
    std::vector<std::string> data;
    Check(data.begin(), data.end(), "", data.begin());
}

TEST_CASE("Small") {
    std::vector one = {1};
    Check(one.begin(), one.end(), 1, one.begin());
    Check(one.begin(), one.end(), 0, one.end());
}

class Int {
public:
    explicit Int(int x) : x_{x} {
    }
    Int(const Int&) = delete;
    Int(Int&&) = delete;
    Int& operator=(const Int&) = delete;
    Int& operator=(Int&&) = delete;

    bool operator==(const Int&) const = default;

private:
    int x_;
};

TEST_CASE("EqualOnly") {
    std::list<Int> elems;
    for (auto x : {1, 2, 1, 3}) {
        elems.emplace_back(x);
    }
    Check(elems.begin(), elems.end(), Int{1}, std::next(elems.begin(), 2));
}

TEST_CASE("Long") {
    constexpr auto kSize = 5'000;
    constexpr auto kVal = 1'000;

    RandomGenerator rnd{85'475'122};
    std::vector<int> elems(kSize);
    std::unordered_map<int, int> last_positions;
    for (auto i : std::views::iota(0, kSize)) {
        elems[i] = rnd.GenInt(-kVal, kVal);
        last_positions[elems[i]] = i;
    }

    auto first = elems.begin();
    auto last = elems.end();
    for (auto i : std::views::iota(-kVal, kVal + 1)) {
        auto map_it = last_positions.find(i);
        auto expected = (map_it == last_positions.end()) ? last : first + map_it->second;
        Check(first, last, i, expected);
    }
}

TEST_CASE("Ranges") {
    constexpr auto kSize = 1'000'000'000'000'000ll;
    constexpr auto kElement = kSize - 1'234'567;
    constexpr auto kRange = std::views::iota(0ll, kSize);

    Check(kRange.begin(), kRange.end(), kElement, kRange.begin() + kElement);

    auto odd_range = std::views::filter(kRange, [](auto x) { return x % 2; });
    auto expected = std::ranges::prev(odd_range.end(), (kSize - kElement + 1) / 2);
    Check(odd_range.begin(), odd_range.end(), kElement, expected);
}
