#pragma once

#include <stdexcept>

template <class Iterator, class T>
Iterator FindLast(Iterator first, Iterator last, const T& value) {
    if (first == last) {
        return last;
    }

    Iterator it = last;
    do {
        --it;
        if (*it == value) {
            return it;
        }
    } while (it != first);

    return last;
}
