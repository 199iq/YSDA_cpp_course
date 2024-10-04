#pragma once

struct Sum {};

struct Prod {};

struct Concat {};

class Length {};

template <class Iterator, class T, class BinaryOp>
T Fold(Iterator first, Iterator last, T init, BinaryOp func) {
}
