#include "word_count.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Small") {
    CHECK(DifferentWordsCount("hello world e") == 3);
    CHECK(DifferentWordsCount("Test") == 1);
    CHECK(DifferentWordsCount("small,test,") == 2);
}

TEST_CASE("Zero") {
    CHECK(DifferentWordsCount("") == 0);
    CHECK(DifferentWordsCount("    ") == 0);
    CHECK(DifferentWordsCount(",123.") == 0);
    CHECK(DifferentWordsCount("!") == 0);
}

TEST_CASE("Register") {
    CHECK(DifferentWordsCount("hello Hello WORLD w,orld wOrld") == 4);
    CHECK(DifferentWordsCount("a A    a A") == 1);
    CHECK(DifferentWordsCount("    register REGISTER matters 'x'") == 3);
}

TEST_CASE("Some tests") {
    CHECK(DifferentWordsCount("12shit happens ") == 2);
    CHECK(DifferentWordsCount(",,abc acb bac bca     cabCba    ") == 5);
}
