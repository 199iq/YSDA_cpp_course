#include "partition.h"
#include "strict_iterator.h"

#include <string>
#include <vector>
#include <algorithm>
#include <span>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_adapters.hpp>
#include <catch2/generators/catch_generators_random.hpp>

using Catch::Generators::chunk;
using Catch::Generators::random;

void CheckUnorderedEquals(const auto& range, const auto& expected) {
    std::vector data(range.begin(), range.end());
    std::vector expected_data(expected.begin(), expected.end());
    std::ranges::sort(data);
    std::ranges::sort(expected_data);
    CHECK(data == expected_data);
}

template <class T, class Predicate>
void Check(std::vector<T>&& data, Predicate pred) {
    auto copy = data;
    auto it = std::partition(copy.begin(), copy.end(), pred);
    std::span expected_first_part{copy.begin(), it};
    std::span expected_second_part{it, copy.end()};

    auto first = MakeStrict(data.begin(), data.begin(), data.end());
    auto last = MakeStrict(data.begin(), data.end(), data.end());
    it = Partition(first, last, pred).Base();
    std::span first_part{data.begin(), it};
    std::span second_part{it, data.end()};

    CheckUnorderedEquals(first_part, expected_first_part);
    CheckUnorderedEquals(second_part, expected_second_part);
}

TEST_CASE("Small") {
    constexpr auto kEqualsTwo = [](auto x) { return x == 2; };
    Check<int>({1}, kEqualsTwo);
    Check<int>({2}, kEqualsTwo);
    Check<int>({1, 1}, kEqualsTwo);
    Check<int>({1, 2}, kEqualsTwo);
    Check<int>({2, 1}, kEqualsTwo);
    Check<int>({2, 2}, kEqualsTwo);
    Check<int>({3, 4, 5, 6}, kEqualsTwo);
    Check<int>({2, 2, 2, 2, 2, 2}, kEqualsTwo);
    Check<int>({2, 2, 2, 1, 1}, kEqualsTwo);
    Check<int>({1, 1, 2}, kEqualsTwo);
    Check<int>({2, 1, 2, 1}, kEqualsTwo);
    Check<int>({0, 2, 0, 2, 0}, kEqualsTwo);
}

TEST_CASE("Simple") {
    constexpr auto kIsEven = [](auto x) { return x % 2 == 0; };

    Check<int>({2, 1, 2, 2, 3, 5, 4}, kIsEven);
    Check<int>({1, 3, 5, 7}, kIsEven);
    Check<int>({2, -2, 4, 8, 10}, kIsEven);

    std::vector<int> v;
    CHECK(Partition(v.begin(), v.end(), kIsEven) == v.end());
}

TEST_CASE("Empty") {
    Check<int>({}, [](auto) { return false; });
    Check<std::string>({"aba"}, [](const auto& s) { return s.size() < 3; });
    Check<std::string>({"aba"}, [](const auto& s) { return s.size() <= 3; });
}

TEST_CASE("Big") {
    auto data = chunk(100'000, random(-1'000'000'000, 1'000'000'000)).get();
    Check(std::move(data), [](auto x) { return x < 0; });
}
