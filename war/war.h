#pragma once

#include <array>
#include <stdexcept>

enum class Winner { kFirst, kSecond, kNone };

struct GameResult {
    Winner winner;
    int turn;
};

GameResult SimulateWarGame(const std::array<int, 6>& first_deck,
                           const std::array<int, 6>& second_deck) {
    throw std::runtime_error{"Not implemented"};
}
