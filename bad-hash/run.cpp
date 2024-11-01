#include "../tools/util/util.h"

#include <iostream>
#include <unordered_set>
#include <string>
#include <random>
#include <ranges>
#include <algorithm>
#include <chrono>

bool CheckUserName(const std::string& user) {
    return std::ranges::all_of(
        user, [](char c) { return std::isalpha(c) || std::isdigit(c) || (c == '_'); });
}

bool GetLine(std::string* line, size_t max_length) {
    line->clear();
    while (std::cin && (line->size() <= max_length)) {
        if (auto symbol = std::cin.get(); symbol == '\n') {
            return true;
        } else {
            line->push_back(symbol);
        }
    }
    return false;
}

std::vector<std::string> GetUsers(size_t max_count) {
    std::vector<std::string> users;
    std::string cur_line;
    while (users.size() < max_count) {
        if (auto is_valid = GetLine(&cur_line, 15u); std::cin.eof()) {
            break;
        } else if (!is_valid) {
            std::cout << "Too long username " << cur_line << "\n";
            std::exit(0);
        } else if (!CheckUserName(cur_line)) {
            std::cout << "Incorrect username " << cur_line << "\n";
            std::exit(0);
        } else {
            users.push_back(cur_line);
        }
    }
    return users;
}

int main() {
    std::random_device gen;
    if (!gen.entropy()) {
        std::cout << "Bad random device\n";
        std::exit(0);
    }
    std::uniform_int_distribution dist{30'000, 1'000'000};

    std::unordered_set<std::string> users(dist(gen));
    auto users_list = GetUsers(15'000);
    Timer timer;

    for (auto& user : users_list) {
        users.insert(std::move(user));
    }

    auto [wall_time, cpu_time] = timer.GetTimes();
    auto spent = std::chrono::duration<double>{wall_time};
    std::cout << "Spent " << spent.count() << " seconds\n";

    if (wall_time > std::chrono::milliseconds{400}) {
        if (cpu_time < .98 * wall_time) {
            std::cout << "Sorry, but CI is laggy 😥\n";
            std::cout << "Try later...\n";
        } else {
            std::cout << "Shit happens\n";
            return 1;
        }
    }
}
