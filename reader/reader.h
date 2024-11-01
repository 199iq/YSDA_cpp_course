#pragma once

#include <string>
#include <algorithm>
#include <ranges>
#include <span>
#include <random>

class Reader {
public:
    virtual ~Reader() = default;

    // Читает не более чем @len байт из потока.
    // Помещает результат чтения в буффер @buf.
    // Возвращает сколько байт удалось прочитать.
    // Возвращаемое значение 0 означает конец потока.

    virtual size_t Read(char* buf, size_t len) = 0;
};

std::string ReadAll(Reader* reader) {
    constexpr auto kChunkSize = 128;

    std::string buf;
    char chunk[kChunkSize];
    while (true) {
        if (auto size = reader->Read(chunk, kChunkSize)) {
            buf.append(chunk, size);
        } else {
            return buf;
        }
    }
}

class EndlessReader : public Reader {
public:
    size_t Read(char* buf, size_t len) override {
        for (auto& c : std::span{buf, len}) {
            c = dist_(gen_);
        }
        return len;
    }

private:
    std::mt19937 gen_{42};
    std::uniform_int_distribution<int> dist_{'a', 'z'};
};

class StringReader : public Reader {
public:
    explicit StringReader(std::string data) : data_{std::move(data)}, it_{data_.begin()} {
    }
    StringReader(const StringReader&) = delete;

    size_t Read(char* buf, size_t len) override {
        auto read_len = std::min<size_t>(len, data_.end() - it_);
        // Reader может вернуть менее @len байт
        read_len = std::min<size_t>(read_len, 100);
        it_ = std::ranges::copy_n(it_, read_len, buf).in;
        return read_len;
    }

private:
    const std::string data_;
    std::string::const_iterator it_;
};
