#pragma once

#include <string>
#include <unordered_map>
#include <filesystem>

using Map = std::unordered_map<std::string, double>;

Map ReadNgrams(const std::filesystem::path& path) {
    return {};
}

std::string Decode(const std::string& text, const Map& dict,
                   const std::filesystem::path& path_letters) {
    return "";
}
