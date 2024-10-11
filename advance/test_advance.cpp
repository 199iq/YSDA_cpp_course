#include "advance.h"

#include <vector>
#include <list>
#include <sstream>
#include <string>
#include <forward_list>
#include <chrono>
#include <iterator>

#define CATCH_CONFIG_NO_EXPERIMENTAL_STATIC_ANALYSIS_SUPPORT

#include <catch2/catch_test_macros.hpp>

using namespace std::chrono_literals;

TEST_CASE("Forward") {
    std::forward_list<int> l = {1, 2, 3, 4, 5};

    auto it = l.begin();
    Advance(it, 2);
    REQUIRE(*it == 3);

    Advance(it, 0);
    REQUIRE(*it == 3);
}

TEST_CASE("Bidirectional") {
    std::list<std::string> l = {"ab", "ba", "aba", "caba"};

    auto first = l.cbegin();
    Advance(first, 3);
    CHECK(*first == "caba");

    auto second = l.cend();
    Advance(second, -3);
    REQUIRE(*second == "ba");

    Advance(second, 0);
    REQUIRE(*second == "ba");
}

TEST_CASE("RandomAccess") {
    std::vector<int> l = {1, 3, 5, 7, 9};

    auto first = l.cbegin();
    Advance(first, 1);
    CHECK(*first == 3);

    auto second = l.cend();
    Advance(second, -4);
    CHECK(*second == 3);

    auto it = l.cbegin();
    Advance(it, l.size());
    CHECK(it == l.end());
}

TEST_CASE("RandomAccessRaw") {
    int l[] = {-1, 1, 0, 4, 8};

    const auto* it = l;
    Advance(it, 2);
    REQUIRE(*it == 0);

    Advance(it, -1);
    REQUIRE(*it == 1);
}

class CheckIterator {
public:
    using value_type = int;
    using difference_type = std::ptrdiff_t;
    using pointer = int*;
    using reference = int&;
    using iterator_category = std::random_access_iterator_tag;
    using iterator_concept = std::random_access_iterator_tag;

    reference operator*() const {
        static int x = 0;
        return x;
    }

    reference operator[](difference_type) const {
        static int x = 0;
        return x;
    }

    CheckIterator& operator++() {
        return (*this) += 1;
    }

    CheckIterator operator++(int) {
        auto prev = *this;
        ++(*this);
        return prev;
    }

    CheckIterator& operator--() {
        return (*this) -= 1;
    }

    CheckIterator operator--(int) {
        auto prev = *this;
        --(*this);
        return prev;
    }

    CheckIterator& operator+=(difference_type n) {
        ++was_called_;
        value_ += n;
        return *this;
    }

    CheckIterator& operator-=(difference_type n) {
        ++was_called_;
        value_ -= n;
        return *this;
    }

    CheckIterator operator+(difference_type n) const {
        auto it = *this;
        return it += n;
    }

    CheckIterator operator-(difference_type n) const {
        auto it = *this;
        return it -= n;
    }

    friend CheckIterator operator+(difference_type n, const CheckIterator& it) {
        return it + n;
    }

    friend CheckIterator operator-(difference_type n, const CheckIterator& it) {
        return it - n;
    }

    difference_type operator-(const CheckIterator& other) const {
        return value_ - other.value_;
    }

    auto operator<=>(const CheckIterator& other) const = default;

    int CalledCount() const {
        return was_called_;
    }

    ptrdiff_t GetValue() const {
        return value_;
    }

private:
    int was_called_ = 0;
    ptrdiff_t value_ = 0;
};

TEST_CASE("RandomAccessFast") {
    for (auto value : {-100, 100}) {
        CheckIterator it;
        Advance(it, value);
        REQUIRE(it.GetValue() == value);
        REQUIRE(it.CalledCount() == 1);
    }

    std::vector<int> v(100'000);
    auto start = std::chrono::steady_clock::now();
    for (auto i = 0u; i < v.size(); ++i) {
        v[i] = i;
        const auto* begin = v.data();
        Advance(begin, i);
        REQUIRE(*begin == static_cast<int>(i));
    }
    auto diff = std::chrono::steady_clock::now() - start;
    REQUIRE(diff < 300ms);
}

TEST_CASE("Input") {
    std::stringstream stream{"1 2 3 4 5"};
    std::istream_iterator<int> it{stream};
    Advance(it, 3);
    REQUIRE(*it == 4);
}

TEST_CASE("Output") {
    std::vector<int> dummy;
    auto it = std::back_inserter(dummy);
    Advance(it, 1);
}
