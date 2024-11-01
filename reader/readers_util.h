#pragma once

#include "reader.h"

#include <vector>
#include <memory>

class LimitReader : public Reader {
public:
    LimitReader(std::unique_ptr<Reader> reader, size_t limit) {
    }

    size_t Read(char* buf, size_t len) override {
        return 0;
    }
};

class TeeReader : public Reader {
public:
    explicit TeeReader(std::vector<std::unique_ptr<Reader>> readers) {
    }

    size_t Read(char* buf, size_t len) override {
        return 0;
    }
};

class HexDecodingReader : public Reader {
public:
    explicit HexDecodingReader(std::unique_ptr<Reader> reader) {
    }

    size_t Read(char* buf, size_t len) override {
        return 0;
    }
};
