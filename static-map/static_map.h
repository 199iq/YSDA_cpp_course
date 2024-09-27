#pragma once

#include <vector>
#include <string>
#include <utility>

class StaticMap {
public:
    explicit StaticMap(const std::vector<std::pair<std::string, std::string>>& items) {
    }

    bool Find(const std::string& key, std::string* value) const {
        return false;
    }
};
