#include "immutable_vector.h"

#include <string>
#include <vector>
#include <random>
#include <concepts>
#include <algorithm>
#include <ranges>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>

template <class... Args>
auto MakeVector(Args&&... args) {
    return std::make_unique<const Vector<int>>(std::forward<Args>(args)...);
}

template <class T>
void RequireEquals(const Vector<T>& data, const std::vector<std::type_identity_t<T>>& expected) {
    REQUIRE(data.Size() == expected.size());
    std::vector<T> result;
    result.reserve(data.Size());
    for (auto i : std::views::iota(size_t{}, data.Size())) {
        result.push_back(data.Get(i));
    }
    REQUIRE_THAT(result, Catch::Matchers::Equals(expected));
}

TEST_CASE("Constructors") {
    const Vector<int> empty;
    REQUIRE(empty.Size() == 0);

    const Vector from_list = {1, 2, 3};
    RequireEquals(from_list, {1, 2, 3});

    std::vector<std::string> origin = {"aba", "caba"};
    const Vector range(origin.begin(), origin.end());
    RequireEquals(range, origin);
}

TEST_CASE("ChangeSize") {
    constexpr auto kRange = std::views::iota(0, 2'000);

    std::vector<std::unique_ptr<const Vector<int>>> versions;
    versions.push_back(MakeVector());
    std::vector<int> expected;
    for (auto i : kRange) {
        auto data = MakeVector(versions.back()->PushBack(i));
        auto& r = versions.emplace_back(std::move(data));
        REQUIRE(r->Size() == static_cast<size_t>(i + 1));
        expected.push_back(i);
        RequireEquals(*r, expected);
    }

    for (auto i : std::views::reverse(kRange)) {
        auto data = MakeVector(versions.back()->PopBack());
        versions.pop_back();
        REQUIRE(data->Size() == static_cast<size_t>(i));
        expected.pop_back();
        RequireEquals(*data, expected);
    }
}

TEST_CASE("Immutable") {
    const Vector<int> vector_one = {1, 2, 3};
    RequireEquals(vector_one, {1, 2, 3});

    const auto vector_two = vector_one.Set(0, 10);
    RequireEquals(vector_two, {10, 2, 3});
    RequireEquals(vector_one, {1, 2, 3});
}

TEST_CASE("SetGet") {
    constexpr auto kRange = std::views::iota(0, 1'000);
    constexpr auto kSize = 50;

    auto data = MakeVector(kSize);
    std::vector<int> vector_data(kSize);
    std::mt19937 gen{7'346'475};
    std::uniform_int_distribution dist{0, kSize - 1};
    for (auto i : kRange) {
        auto index = dist(gen);
        data = MakeVector(data->Set(index, i + 1));
        vector_data[index] = i + 1;
        for (auto j = 0; j < 10; ++j) {
            auto check_index = dist(gen);
            REQUIRE(data->Get(check_index) == vector_data[check_index]);
        }
    }
    RequireEquals(*data, vector_data);
}

TEST_CASE("PushBack") {
    auto data = MakeVector();
    std::vector<int> ok_data;
    for (auto i : std::views::iota(0, 100)) {
        ok_data.push_back(i);
        data = MakeVector(data->PushBack(i));
        RequireEquals(*data, ok_data);
    }
}

TEST_CASE("Mix") {
    constexpr auto kRange = std::views::iota(0, 1'000);

    auto data = MakeVector(kRange.begin(), kRange.end());
    std::mt19937 gen{7'464'754};
    for (auto j = 0; j < 10; ++j) {
        const auto my_data = *data;
        std::uniform_int_distribution dist{0, static_cast<int>(my_data.Size()) - 1};
        for ([[maybe_unused]] auto k : kRange) {
            auto index = dist(gen);
            REQUIRE(my_data.Get(index) == index);
        }
    }

    for ([[maybe_unused]] auto i : kRange) {
        data = MakeVector(data->PopBack());
    }
    REQUIRE(data->Size() == 0);
}

TEST_CASE("BigTest") {
    constexpr auto kIterationsCount = 100'000;

    auto data = MakeVector();
    std::mt19937 gen{93'475};
    std::vector<int> indices;
    indices.reserve(kIterationsCount);
    for (auto i : std::views::iota(0, kIterationsCount)) {
        data = MakeVector(data->PushBack(i));
        indices.push_back(i);
    }
    std::ranges::shuffle(indices, gen);
    for (auto i : std::views::iota(0, kIterationsCount / 2)) {
        data = MakeVector(data->Set(indices[i], -1));
    }

    for (auto i : std::views::iota(size_t{}, data->Size())) {
        auto cur_value = data->Get(indices[i]);
        if (i < kIterationsCount / 2) {
            REQUIRE(cur_value == -1);
        } else {
            REQUIRE(cur_value == indices[i]);
        }
    }
}

TEST_CASE("Performance") {
    constexpr auto kSize = 50'000;
    constexpr auto kRange = std::views::iota(0, kSize);
    constexpr auto kInitSize = 100'000;
    constexpr auto kInitRange = std::views::iota(0, kInitSize);
    std::mt19937 gen{886'578};

    std::vector<std::unique_ptr<const Vector<int>>> versions;
    versions.emplace_back(MakeVector(kInitRange.begin(), kInitRange.end()));
    for (auto i : kRange) {
        auto v = MakeVector(versions.back()->PushBack(i + kInitSize));
        versions.push_back(std::move(v));
    }

    for (auto i : kRange) {
        const auto& v = *versions[i + 1];
        REQUIRE(v.Get(1'234) == 1'234);
        REQUIRE(v.Get(kInitSize + i) == kInitSize + i);
        REQUIRE(v.Size() == static_cast<size_t>(kInitSize) + i + 1);
    }

    auto data = MakeVector(*versions.back());
    for (auto i : std::views::iota(0, kInitSize + kSize) | std::views::reverse) {
        REQUIRE(data->Size() == static_cast<size_t>(i) + 1);
        auto x = std::uniform_int_distribution{0, i}(gen);
        REQUIRE(data->Get(x) == x);
        data = MakeVector(data->PopBack());
    }
    REQUIRE(data->Size() == 0);
}

void CheckSharedState(const Vector<int>& a, const Vector<int>& b) {
    REQUIRE(a.Size() == b.Size());
    for (auto i : std::views::iota(size_t{}, a.Size())) {
        REQUIRE(&a.Get(i) == &b.Get(i));
    }
}

TEST_CASE("Move doesn't change state") {
    using V = Vector<int>;
    STATIC_CHECK(std::copy_constructible<V>);
    STATIC_CHECK(std::move_constructible<V>);
    STATIC_CHECK_FALSE(std::assignable_from<V&, const V&>);
    STATIC_CHECK_FALSE(std::assignable_from<V&, V&&>);

    std::vector expected = {1, 2, 3, 4, 5};
    Vector<int> a(expected.begin(), expected.end());
    const auto b = a;
    RequireEquals(a, expected);
    CheckSharedState(a, b);

    const auto c = std::move(a);
    RequireEquals(a, expected);
    CheckSharedState(a, c);
}
