#pragma once

#include <filesystem>
#include <string>

std::filesystem::path GetRelativeDir(const std::string& file, const std::string& relative_subdir) {
    return std::filesystem::path(file).parent_path() / relative_subdir;
}