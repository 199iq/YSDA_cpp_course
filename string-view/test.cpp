#include "string_view.h"
#include "util.h"

#include <string>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Constructors") {
    {
        const auto* cs = "abacaba";
        StringView sv = cs;
        CHECK(sv[3] == 'c');
        CHECK(sv.Data() == cs);
        CHECK(sv.Size() == 7);
    }
    {
        const auto* cs = "caba";
        StringView sv{cs, 3};
        CHECK(sv[0] == 'c');
        CHECK(sv.Data() == cs);
        CHECK(sv.Size() == 3);
    }
    {
        std::string s = "abacaba";
        StringView sv = s;
        CHECK(sv[3] == 'c');
        CHECK(sv.Data() == s.data());
        CHECK(sv.Size() == 7);
    }
    {
        std::string s = "abacaba";
        StringView sv{s, 3};
        CHECK(sv[2] == 'b');
        CHECK(sv.Data() == s.data() + 3);
        CHECK(sv.Size() == 4);
    }
    {
        std::string s = "abacaba";
        StringView sv{s, 3, 3};
        CHECK(sv[1] == 'a');
        CHECK(sv.Data() == s.data() + 3);
        CHECK(sv.Size() == 3);
    }
}

TEST_CASE("Constness") {
    const std::string s = "abacaba";
    const StringView sv{s, 2, 3};
    CHECK(sv[2] == 'a');
    CHECK(sv.Data() == s.data() + 2);
    CHECK(sv.Size() == 3);
}

TEST_CASE("Change source") {
    {
        std::string s = "abacaba";
        const StringView sv{s, 4};
        s[5] = 'd';
        CHECK(sv[1] == 'd');
    }
    {
        char cs[] = "abacaba";
        StringView sv{cs + 1, 3};
        cs[3] = 'f';
        CHECK(sv[2] == 'f');
    }
}

TEST_CASE("Big") {
    constexpr auto kSize = 100'000;

    RandomGenerator rnd{38'545'678};
    auto s = rnd.GenString(kSize);
    StringView sv{s, 1};

    for (auto i = 0; i < kSize; ++i) {
        auto ind = rnd.GenInt(1, kSize - 1);
        s[ind] = rnd.GenInt<int>('a', 'z');
        REQUIRE(s[ind] == sv[ind - 1]);
    }
    REQUIRE(sv.Size() == kSize - 1);
}
