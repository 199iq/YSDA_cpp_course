#include "decoder.h"
#include "util.h"

#include <string>
#include <filesystem>
#include <fstream>
#include <sstream>

#include <catch2/catch_test_macros.hpp>

std::string ReadText(const std::filesystem::path& path) {
    std::ifstream in{path};
    if (!in) {
        throw std::runtime_error{"Can't open file: " + path.string()};
    }
    std::ostringstream buffer;
    buffer << in.rdbuf();
    return buffer.str();
}

// static const auto kDictionary = ReadNgrams(GetFileDir(__FILE__) / "english_quadgrams.txt");

TEST_CASE("Your test") {
}
