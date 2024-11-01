#include <cstdlib>
#include <string>
#include <iostream>

const std::string kAlphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

void GenString(std::string* s) {
    for (auto& c : *s) {
        c = kAlphabet[std::rand() % kAlphabet.size()];
    }
}

bool ReadString(std::string* line, size_t max_length) {
    line->clear();
    while (std::cin && (line->size() <= max_length)) {
        if (auto symbol = std::cin.get(); symbol != '\n') {
            line->push_back(symbol);
        } else {
            return true;
        }
    }
    return false;
}

uint32_t ReadSeed(char* arg) {
    try {
        return std::stoul(arg);
    } catch (const std::exception&) {
        std::cout << "Failed to read seed\n";
        std::exit(0);
    }
}

int main(int argc, char* argv[]) {
    auto seed = (argc >= 2) ? ReadSeed(argv[1]) : 2344346u;
    std::srand(seed);

    std::string password(10, 'a');
    for (auto i = 0; i < 10'000'000; ++i) {
        GenString(&password);
    }

    if (password != "NLXGI4NoAp") {
        std::cout << "Wrong seed\n";
        return 0;
    }

    GenString(&password);
    std::string guess;
    if (!ReadString(&guess, 10u)) {
        std::cout << "Too long string\n";
        return 0;
    }
    if (password == guess) {
        std::cout << "Wow so hacker\n";
        return 1;
    }
}
