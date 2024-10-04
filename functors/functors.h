#pragma once

#include <cstddef>

template <class Functor>
class ReverseUnaryFunctor {};

template <class Functor>
class ReverseBinaryFunctor {};

auto MakeReverseUnaryFunctor(auto functor) {
    return ReverseUnaryFunctor{functor};
}

auto MakeReverseBinaryFunctor(auto functor) {
    return ReverseBinaryFunctor{functor};
}

template <class Iterator>
size_t ComparisonsCount(Iterator first, Iterator last);
