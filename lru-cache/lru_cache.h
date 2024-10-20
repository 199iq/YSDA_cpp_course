#pragma once

#include <string>

class LruCache {
public:
    explicit LruCache(size_t max_size) {
    }
    LruCache(const LruCache&) = delete;
    LruCache& operator=(const LruCache&) = delete;

    void Set(const std::string& key, const std::string& value) {
    }

    bool Get(const std::string& key, std::string* value) {
        return false;
    }

private:
};
