#pragma once

#include <cstddef>
#include <vector>
#include <iostream>
#include <utility>

template <class Iterator>
class Sequence {
public:
    using const_iterator = Iterator;
    Sequence(Iterator begin, Iterator end) : begin_{begin}, end_{end} {
    }

    Iterator begin() const {
        return begin_;
    }

    Iterator end() const {
        return end_;
    }

private:
    Iterator begin_, end_;
};


template <class FirstIterator, class SecondIterator>
class ZipIterator {
public:
    ZipIterator(FirstIterator first, SecondIterator second)
        : first_(first), second_(second) {
    }

    FirstIterator first() const {
        return first_;
    }
    SecondIterator second() const {
        return second_;
    }

    ZipIterator& operator++() {
        ++first_;
        ++second_;
        return *this;
    }

    auto operator*() const {
        return std::pair{*first_, *second_};
    }

    bool operator==(const ZipIterator& rhs) const {
        return (first_ == rhs.first() || second_ == rhs.second());
    }
    // TODO: maybe try to set the default overloading for != operator

private:
    FirstIterator first_;
    SecondIterator second_;
};

template <class Iterator>
class RangeIterator {
public:
    RangeIterator(Iterator it, int64_t step)
        : it_(it), step_(step) {
    }

    bool operator!=(const RangeIterator& rhs) const {
        // suppose that a != b <-> a < b
        return it_ < rhs.it_; // will this work ???
    }

    bool operator==(const RangeIterator& rhs) const = default;

    auto operator*() const {
        return it_;
    }

    RangeIterator& operator++() {
        it_ += step_;
        return *this;
    }

private:
    Iterator it_; // currently it_ is just a number
    int64_t step_;
};

template <class Iterable>
auto Group(const Iterable& seq) {
    std::vector<Sequence<typename Iterable::const_iterator>> res;
    
    if (seq.begin() == seq.end()) {
        // seq is empty
        return res;
    }

    auto begin = seq.begin(), next = begin;
    ++next;

    for (auto it = next; it != seq.end(); ++it) {
        if (*it != *begin) {
            res.emplace_back(begin, it);
            begin = it;
        }
    }
    res.emplace_back(begin, seq.end()); // last segment
    return res;
}

auto Range(int64_t from, int64_t to, int64_t step) {
    auto begin = RangeIterator(from, step), end = RangeIterator(to, step);
    return Sequence(begin, end);
}

// overload the Range function
auto Range(int64_t from, int64_t to) {
    return Range(from, to, 1ll); // step = 1
}

auto Range(int64_t to) {
    return Range(0ll, to, 1ll); // from = 0, step = 1
}

auto Zip(const auto& f_seq, const auto& s_seq) {
    auto f_begin = f_seq.begin(), f_end = f_seq.end();
    auto s_begin = s_seq.begin(), s_end = s_seq.end();

    auto seq_begin = ZipIterator(f_begin, s_begin);
    auto seq_end = ZipIterator(f_end, s_end);

    return Sequence(seq_begin, seq_end);
}

template <class Iterator>
class RepeatIterator {
public:
    RepeatIterator(Iterator begin, Iterator end, size_t count = 0)
        : begin_{begin}, it_{begin}, end_{end}, count_{count} {
    }

    RepeatIterator& operator++() {
        if (!(++it_ != end_)) {
            ++count_;
            it_ = begin_;
        }
        return *this;
    }

    auto operator*() const {
        return *it_;
    }

    bool operator==(const RepeatIterator& rhs) const = default;

private:
    Iterator begin_, it_, end_;
    size_t count_;
};

auto Repeat(const auto& sequence, size_t n) {
    auto begin = sequence.begin();
    auto end = sequence.end();
    if (n && (begin != end)) {
        return Sequence{RepeatIterator{begin, end}, RepeatIterator{begin, end, n}};
    } else {
        return Sequence{RepeatIterator{end, end}, RepeatIterator{end, end}};
    }
}
