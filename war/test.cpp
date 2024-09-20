#include "war.h"

#include <catch2/catch_test_macros.hpp>

using Deck = std::array<int, 6>;

void Check(const Deck& first_deck, const Deck& second_deck, Winner winner, int turn = 0) {
    auto result = SimulateWarGame(first_deck, second_deck);
    CHECK(result.winner == winner);
    if (turn) {
        CHECK(result.turn == turn);
    }
}

TEST_CASE("Simple") {
    Check({2, 4, 6, 8, 10, 0}, {1, 3, 5, 7, 9, 11}, Winner::kFirst, 6);
    Check({1, 3, 5, 7, 9, 11}, {2, 4, 6, 8, 10, 0}, Winner::kSecond, 6);
}

TEST_CASE("Short") {
    Check({0, 7, 8, 10, 2, 9}, {4, 11, 6, 3, 5, 1}, Winner::kFirst, 18);
    Check({0, 3, 11, 9, 8, 10}, {5, 4, 1, 7, 2, 6}, Winner::kFirst, 24);
    Check({8, 3, 0, 4, 1, 10}, {2, 7, 9, 5, 6, 11}, Winner::kSecond, 16);
    Check({2, 0, 11, 6, 4, 3}, {5, 1, 8, 10, 9, 7}, Winner::kSecond, 20);
}

TEST_CASE("Long") {
    Check({5, 0, 9, 2, 8, 1}, {4, 7, 3, 11, 10, 6}, Winner::kFirst, 316);
    Check({5, 6, 9, 2, 10, 4}, {1, 8, 0, 7, 11, 3}, Winner::kFirst, 346);
    Check({9, 5, 1, 2, 4, 10}, {11, 6, 0, 8, 3, 7}, Winner::kSecond, 200);
    Check({10, 7, 1, 11, 9, 6}, {4, 8, 2, 3, 5, 0}, Winner::kSecond, 208);
}

TEST_CASE("Inf") {
    Check({0, 2, 8, 11, 7, 3}, {4, 5, 9, 10, 6, 1}, Winner::kNone);
    Check({1, 7, 2, 11, 6, 10}, {0, 3, 8, 4, 9, 5}, Winner::kNone);
    Check({1, 11, 4, 0, 7, 10}, {2, 9, 5, 6, 8, 3}, Winner::kNone);
    Check({10, 0, 6, 9, 7, 5}, {11, 1, 4, 2, 3, 8}, Winner::kNone);
}
