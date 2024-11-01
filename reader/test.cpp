#include "reader.h"
#include "readers_util.h"
#include "util.h"

#include <algorithm>

#include <catch2/catch_test_macros.hpp>

std::unique_ptr<Reader> MakeStringReader(std::string data) {
    return std::make_unique<StringReader>(std::move(data));
}

TEST_CASE("StringReader") {
    CHECK(ReadAll(MakeStringReader("").get()).empty());
    CHECK(ReadAll(MakeStringReader("a").get()) == "a");

    std::string big_string(10'000, 'f');
    CHECK(ReadAll(MakeStringReader(big_string).get()) == big_string);
}

TEST_CASE("LimitReader") {
    {
        LimitReader l{MakeStringReader(""), 0};
        CHECK(ReadAll(&l).empty());
    }
    {
        LimitReader l{MakeStringReader(" "), 0};
        CHECK(ReadAll(&l).empty());
    }
    {
        LimitReader l{MakeStringReader("ab"), 1};
        CHECK(ReadAll(&l) == "a");
    }
    {
        LimitReader l{MakeStringReader("abacaba"), 5};
        CHECK(ReadAll(&l) == "abaca");
    }
    {
        LimitReader l{MakeStringReader("foo"), 10};
        CHECK(ReadAll(&l) == "foo");
    }
    {
        LimitReader l{MakeStringReader("bar"), 3};
        CHECK(ReadAll(&l) == "bar");
    }
    {
        auto str = RandomGenerator{42}.GenString(500, 'A', 'A' + 9);
        LimitReader l{MakeStringReader(str), 123};
        CHECK(ReadAll(&l) == str.substr(0, 123));
    }
    {
        auto big_string = RandomGenerator{459}.GenString(1'000'000);
        LimitReader l{MakeStringReader(big_string), 645'141};
        CHECK(ReadAll(&l) == big_string.substr(0, 645'141));
    }
}

void CheckDoubleLimit(std::string s, size_t n1, size_t n2) {
    auto copy = s;
    auto size = std::min({s.size(), n1, n2});
    auto l1 = std::make_unique<LimitReader>(MakeStringReader(std::move(s)), n1);
    LimitReader l2{std::move(l1), n2};
    CHECK(ReadAll(&l2) == copy.substr(0, size));
}

TEST_CASE("LimitOnLimit") {
    for (auto n1 : std::views::iota(0, 10)) {
        for (auto n2 : std::views::iota(0, 10)) {
            CheckDoubleLimit("abacaba", n1, n2);
        }
    }
}

TeeReader MakeTee(std::vector<std::string> chunks) {
    std::vector<std::unique_ptr<Reader>> readers;
    for (auto& c : chunks) {
        readers.push_back(MakeStringReader(std::move(c)));
    }
    return TeeReader{std::move(readers)};
}

TEST_CASE("TeeReader") {
    {
        TeeReader t{{}};
        CHECK(ReadAll(&t).empty());
    }
    {
        auto t = MakeTee({""});
        CHECK(ReadAll(&t).empty());
    }
    {
        auto t = MakeTee({"abc"});
        CHECK(ReadAll(&t) == "abc");
    }
    {
        auto t = MakeTee({"abc", "def", "g"});
        CHECK(ReadAll(&t) == "abcdefg");
    }
    {
        auto t = MakeTee({"abc", "", "def"});
        CHECK(ReadAll(&t) == "abcdef");
    }
    {
        auto t = MakeTee({"abc", "", "", "45", "", "def", "12", ""});
        CHECK(ReadAll(&t) == "abc45def12");
    }
    {
        auto big_string = RandomGenerator{43}.GenString(500, 'A', 'A' + 9);
        auto t = MakeTee({big_string, big_string, big_string});
        CHECK(ReadAll(&t) == big_string + big_string + big_string);
    }
    {
        auto t = MakeTee({"", "", ""});
        CHECK(ReadAll(&t).empty());
    }
}

TEST_CASE("HexReader") {
    HexDecodingReader h1{MakeStringReader("")};
    CHECK(ReadAll(&h1).empty());

    HexDecodingReader h2{MakeStringReader("616263")};
    CHECK(ReadAll(&h2) == "abc");

    std::string big_hex = "74657374666f6f626172";
    std::string answer = "testfoobar";
    for (auto i = 0; i < 6; ++i) {
        big_hex += big_hex;
        answer += answer;
    }

    HexDecodingReader h3{MakeStringReader(big_hex)};
    CHECK(ReadAll(&h3) == answer);
}

std::unique_ptr<Reader> MakeEndless() {
    return std::make_unique<EndlessReader>();
}

TEST_CASE("TeeWithLimit") {
    static constexpr auto kLimit = 100'000;

    RandomGenerator gen{5'469};
    std::string prefix = "aba";
    auto s1 = gen.GenString(455'573);
    auto s2 = gen.GenString(56'153);
    auto s3 = gen.GenString(341'646);
    std::string suffix = "caba";
    auto expected =
        prefix + s1.substr(0, kLimit) + s2.substr(0, kLimit) + s3.substr(0, kLimit) + suffix;

    auto l1 = std::make_unique<LimitReader>(MakeStringReader(std::move(s1)), kLimit);
    auto l2 = std::make_unique<LimitReader>(MakeStringReader(std::move(s2)), kLimit);
    auto l3 = std::make_unique<LimitReader>(MakeStringReader(std::move(s3)), kLimit);

    std::vector<std::unique_ptr<Reader>> readers;
    readers.push_back(MakeStringReader(std::move(prefix)));
    readers.push_back(std::move(l1));
    readers.push_back(std::move(l2));
    readers.push_back(std::move(l3));
    readers.push_back(MakeStringReader(std::move(suffix)));
    TeeReader t{std::move(readers)};
    CHECK(ReadAll(&t) == expected);
}

TEST_CASE("Endless reader") {
    static constexpr auto kLimit = 1'000'000;

    LimitReader l1{MakeEndless(), kLimit};
    CHECK(ReadAll(&l1).size() == kLimit);

    auto l2 = std::make_unique<LimitReader>(MakeEndless(), kLimit);
    auto l3 = std::make_unique<LimitReader>(MakeEndless(), 2 * kLimit);
    std::vector<std::unique_ptr<Reader>> readers;
    readers.push_back(std::move(l2));
    readers.push_back(std::move(l3));
    TeeReader t{std::move(readers)};
    CHECK(ReadAll(&t).size() == 3 * kLimit);
}

TEST_CASE("Dummy test") {
    static constexpr auto kSize = 1'000;

    std::vector<LimitReader> readers_l;
    std::vector<TeeReader> readers_t;
    char buffer[kSize];
    for (auto i = 0; i < 10'000; ++i) {
        auto& l = readers_l.emplace_back(MakeEndless(), 1'000'000);
        REQUIRE(l.Read(buffer, kSize) == kSize);

        std::vector<std::unique_ptr<Reader>> readers;
        readers.emplace_back(MakeEndless());
        readers.emplace_back(MakeEndless());

        auto& t = readers_t.emplace_back(std::move(readers));
        REQUIRE(t.Read(buffer, kSize) == kSize);
    }
}
