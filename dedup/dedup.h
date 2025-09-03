#pragma once

#include <memory>
#include <vector>
#include <unordered_map>

template <class T>
std::vector<std::unique_ptr<T>> Duplicate(const std::vector<std::shared_ptr<T>>& items) {
    std::vector<std::unique_ptr<T>> ans;

    // warning: items[i] are not nullptr
    for (size_t i = 0; i < items.size(); i++) {
        ans.push_back(std::make_unique<T>(*items[i]));
    }
    return ans;
}

template <class T>
std::vector<std::shared_ptr<T>> DeDuplicate(const std::vector<std::unique_ptr<T>>& items) {
    std::vector<std::shared_ptr<T>> ans;
    std::unordered_map<T, std::shared_ptr<T>> mp;
    // unordered_map is pretty narrow with hashable types
    // so maybe it's better to use std::map to widen key types

    // warning: items[i] are not nullptr
    for (size_t i = 0; i < items.size(); i++) {
        auto it = mp.find(*items[i]);
        if (it == mp.end()) {
            // no such element found
            std::shared_ptr<T> ptr = std::make_shared<T>(*items[i]);
            mp[*items[i]] = ptr;
        }
        ans.push_back(mp[*items[i]]);
    }

    return ans;
}