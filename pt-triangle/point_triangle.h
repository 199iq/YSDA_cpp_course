#pragma once

#include <stdexcept>

struct Point {
    int x, y;
};

struct Triangle {
    Point a, b, c;
};

bool IsPointInTriangle(const Triangle& t, const Point& pt) {
    throw std::runtime_error{"Not implemented"};
}
