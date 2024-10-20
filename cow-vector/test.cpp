#include "cow_vector.h"

#include <ranges>
#include <vector>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Simple vector operations") {
    COWVector v;
    REQUIRE(v.Size() == 0);

    v.Resize(3);
    REQUIRE(v.Size() == 3);
    REQUIRE(v.Get(0).empty());

    v.Set(2, "foo");
    v.Set(1, "bar");

    REQUIRE(v.Get(2) == "foo");
    REQUIRE(v.Get(1) == "bar");

    REQUIRE(v.Back() == "foo");

    v.PushBack("zog");
    REQUIRE(v.Size() == 4);
    REQUIRE(v.Back() == "zog");
}

TEST_CASE("COW semantics") {
    COWVector v1;
    v1.PushBack("foo");
    const auto* p1 = &v1.Get(0);

    SECTION("PushBack unshares state") {
        auto v2 = v1;
        REQUIRE(&v2.Get(0) == p1);

        v2.PushBack("bar");
        REQUIRE(&v2.Get(0) != p1);
        REQUIRE(v2.Get(0) == "foo");
    }

    SECTION("Set unshares state") {
        auto v2 = v1;
        v2.Set(0, "bar");
        const auto* p2 = &v2.Get(0);
        REQUIRE(v2.Back() == "bar");
        REQUIRE(p2 != p1);

        v2.Set(0, "zog");
        REQUIRE(p2 == &v2.Get(0));
    }

    SECTION("Resize") {
        auto v2 = v1;
        v2.Resize(v2.Size() + 1);
        REQUIRE(v2.Back().empty());
        REQUIRE(v2.Get(0) == "foo");
        REQUIRE(&v2.Get(0) != p1);

        auto v3 = v1;
        v3.Resize(v3.Size());
        REQUIRE(&v3.Get(0) == p1);

        auto v4 = v1;
        v4.Resize(0);
        REQUIRE(v4.Size() == 0);
    }

    SECTION("Copy assignment frees old state") {
        COWVector v2;
        v2.PushBack("bar");
        v2.PushBack("zog");
        auto v3 = v2;
        REQUIRE(v3.Back() == "zog");

        v2 = v1;
        REQUIRE(v2.Back() == "foo");
        REQUIRE(v2.Get(0) == "foo");
        REQUIRE(&v2.Get(0) == p1);
        REQUIRE(v3.Get(0) == "bar");
        REQUIRE(v3.Get(1) == "zog");
    }

    SECTION("Read operation do not copy state") {
        auto v2 = v1;
        REQUIRE(v2.Size() == 1);
        REQUIRE(&v2.Get(0) == p1);
    }

    SECTION("Assigment does not change state") {
        const auto& r = v1;
        v1 = r;
        REQUIRE(v1.Size() == 1);
        REQUIRE(&v1.Get(0) == p1);

        auto v2 = v1;
        v1 = v2;
        REQUIRE(v1.Size() == 1);
        REQUIRE(&v1.Get(0) == p1);
    }
}

TEST_CASE("Correct reallocation") {
    COWVector v;
    v.PushBack("foo");
    v.PushBack("foo");
    v.PushBack("foo");
    CHECK(v.Size() == 3);
}

TEST_CASE("Fast copying") {
    constexpr auto kSize = 100'000;

    COWVector v;
    for (auto i : std::views::iota(0, kSize)) {
        v.PushBack(std::to_string(i));
    }

    std::vector<COWVector> vectors;
    for (auto i : std::views::iota(0, kSize)) {
        vectors.push_back(v);
        const auto& value = vectors.back().Get(i);
        if (auto expected = std::to_string(i); value != expected) {
            FAIL(value << " != " << expected);
        }
    }
}

TEST_CASE("Fast assignment") {
    constexpr auto kSize = 100'000;

    COWVector v;
    for (auto i : std::views::iota(0, kSize)) {
        v.PushBack(std::to_string(i));
    }

    std::vector<COWVector> vectors(kSize);
    for (auto i : std::views::iota(0, kSize)) {
        vectors[i] = v;
        const auto& value = vectors[i].Get(i);
        if (auto expected = std::to_string(i); value != expected) {
            FAIL(value << " != " << expected);
        }
    }
}
