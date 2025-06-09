#pragma once

#include <vector>
#include <string>
#include <utility>
#include <iostream>
#include <algorithm>
#include <iterator>

class StaticMap {
public:
    explicit StaticMap(const std::vector<std::pair<std::string, std::string>>& items)
        : map(items) {
            std::sort(map.begin(), map.end());
    }

    bool Find(const std::string& key, std::string* value) const {
        std::pair<std::string, std::string> pivot = {key, ""};
        auto it = std::lower_bound(map.begin(), map.end(), pivot);
        if (it == map.end() || it->first != key) {
            return false;
        }
        *value = it->second;
        return true;
    }
private:
    std::vector<std::pair<std::string, std::string>> map;
};
