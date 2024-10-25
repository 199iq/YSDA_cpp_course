#include "dedup.h"

#include <string>
#include <memory>
#include <vector>
#include <concepts>
#include <ranges>
#include <algorithm>
#include <random>

#include <catch2/catch_test_macros.hpp>

template <class T>
void CheckSignature() {
    using VS = std::vector<std::shared_ptr<T>>;
    using VU = std::vector<std::unique_ptr<T>>;
    STATIC_CHECK(std::same_as<decltype(&Duplicate<T>), VU (*)(const VS&)>);
    STATIC_CHECK(std::same_as<decltype(&DeDuplicate<T>), VS (*)(const VU&)>);
}

TEST_CASE("Signature") {
    CheckSignature<double>();
    CheckSignature<std::string>();
    CheckSignature<int64_t>();
}

TEST_CASE("Duplicating strings") {
    auto i0 = std::make_shared<std::string>("a");
    auto i1 = std::make_shared<std::string>("b");
    auto i2 = std::make_shared<std::string>("c");

    std::vector shared = {i0, i1, i2, i2, i1, i0};

    auto unshared = Duplicate(shared);
    REQUIRE(unshared.size() == 6);

    CHECK(*unshared[0] == "a");
    CHECK(*unshared[1] == "b");
    CHECK(*unshared[2] == "c");
    CHECK(*unshared[3] == "c");
    CHECK(*unshared[4] == "b");
    CHECK(*unshared[5] == "a");

    CHECK(unshared[0] != unshared[5]);
    CHECK(unshared[1] != unshared[4]);
    CHECK(unshared[2] != unshared[3]);

    CHECK(unshared[0].get() != i0.get());
    CHECK(unshared[1].get() != i1.get());
    CHECK(unshared[2].get() != i2.get());
}

TEST_CASE("Deduping strings") {
    std::vector<std::unique_ptr<std::string>> unshared;
    for (auto s : {"a", "b", "c", "c", "b", "a"}) {
        unshared.push_back(std::make_unique<std::string>(s));
    }

    auto shared = DeDuplicate(unshared);
    REQUIRE(shared.size() == 6);

    CHECK(*shared[0] == "a");
    CHECK(*shared[1] == "b");
    CHECK(*shared[2] == "c");
    CHECK(*shared[3] == "c");
    CHECK(*shared[4] == "b");
    CHECK(*shared[5] == "a");

    CHECK(shared[0] == shared[5]);
    CHECK(shared[1] == shared[4]);
    CHECK(shared[2] == shared[3]);
}

TEST_CASE("More duplication") {
    auto item = std::make_shared<std::string>("abacaba");
    std::vector shared(3, item);

    auto unshared = Duplicate(shared);
    REQUIRE(unshared.size() == 3);

    CHECK(unshared[0] != unshared[1]);
    CHECK(unshared[1] != unshared[2]);
    for (const auto& u : unshared) {
        CHECK(*u == "abacaba");
    }
}

TEST_CASE("Big DeDuplicate") {
    constexpr auto kNumUnique = 10'000;
    constexpr auto kNumDuplicates = 100;
    constexpr auto kSize = kNumDuplicates * kNumUnique;

    std::vector<std::unique_ptr<int>> unshared;
    for (auto i : std::views::iota(0, kSize)) {
        unshared.push_back(std::make_unique<int>(i % kNumUnique));
    }
    std::ranges::shuffle(unshared, std::mt19937{45'134});

    auto shared = DeDuplicate(unshared);
    REQUIRE(shared.size() == kSize);
    for (const auto& s : shared) {
        REQUIRE(s.use_count() == kNumDuplicates);
    }

    for (auto i : std::views::iota(0, kSize)) {
        REQUIRE(*shared[i] == *unshared[i]);
    }
}
