#pragma once

#include <stdexcept>
#include <iostream>
#include <cmath>

enum class RootCount { kZero, kOne, kTwo, kInf };

struct Roots {
    RootCount count;
    double first;
    double second;
};

Roots SolveQuadratic(int a, int b, int c) {
    Roots root;

    if (a == 0) {
        // linear equation
        if (b == 0) {
            if (c == 0) {
                root.count = RootCount::kInf;
            } else {
                root.count = RootCount::kZero;
            }
            return root;
        }
        root.count = RootCount::kOne;
        root.first = static_cast<double>(-c) / b;
        return root;
    }

    int64_t disc = b * 1ll * b - 4ll * a * c;

    if (disc > 0) {
        root.count = RootCount::kTwo;
    } else if (disc == 0) {
        root.count = RootCount::kOne;
    } else {
        root.count = RootCount::kZero;
    }

    if (disc >= 0) {
        double sq = sqrt(static_cast<double>(disc));
        root.first = (-b - sq) / (2 * static_cast<double>(a));
        if (disc > 0) {
            root.second = (-b + sq) / (2 * static_cast<double>(a));
        }

        // swap first and second root if we need it
        if (disc > 0 && root.first > root.second) {
            std::swap(root.first, root.second);
        }

        // std::cout << a << " " << b << " " << c << " ::: " << root.first << " ";
        // if (disc > 0) std::cout << root.second;
        // std::cout << "\n";
    }
    return root;
}
