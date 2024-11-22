#include "fake_data.h"
#include "util.h"

#include <fstream>
#include <filesystem>
#include <sstream>
#include <stdexcept>

namespace {

std::string ReadFile(const std::filesystem::path& path) {
    std::ifstream in{path};
    if (!in.is_open()) {
        throw std::runtime_error{"file doesn't exist"};
    }
    std::ostringstream buffer;
    buffer << in.rdbuf();
    return std::move(buffer).str();
}

const auto kDir = GetFileDir(__FILE__);

}  // namespace

const std::string fake_data::kMoscowWeather = ReadFile(kDir / "moscow.json");
const std::string fake_data::kSpbWeather = ReadFile(kDir / "spb.json");
