#pragma once

#include <stdexcept>
#include <iostream>
#include <utility>

template <class Iterator, class Predicate>
Iterator Partition(Iterator first, Iterator last, Predicate pred) {
    Iterator ans = first, next_good = first;
    for (; ans != last; ++ans) {
        while (next_good != last) {
            if (pred(*next_good)) {
                break;
            }
            ++next_good;
        }
        if (next_good == last) {
            break;
        } else {
            if (ans != next_good) {
                std::swap(*ans, *next_good);
            }
            ++next_good;
        }
    }
    return ans;
}
