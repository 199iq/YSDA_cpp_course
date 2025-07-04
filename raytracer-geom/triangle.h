#pragma once

#include "vector.h"

#include <cstddef>
#include <array>
#include <utility>

class Triangle {
public:
    Triangle(const Vector& a, const Vector& b, const Vector& c) {
        data_[0] = a;
        data_[1] = b;
        data_[2] = c;

        ab_ = GetVector(data_[0], data_[1]);
        ac_ = GetVector(data_[0], data_[2]);
    }

    std::pair<Vector, Vector> GetSides() const {
        return {ab_, ac_};
    }

    const Vector& operator[](size_t ind) const {
        if (ind >= 3) {
            throw std::out_of_range("Index out of range");
        }
        return data_[ind];
    }

    const Vector GetNormal() const {
        // get normal to the plane on which triangle is located
        Vector normal = CrossProduct(ab_, ac_);
        normal.Normalize();

        return normal;
    }

    double Area() const {
        double ans = Length(CrossProduct(ab_, ac_)) / 2;
        return ans;
    }

private:
    std::array<Vector, 3> data_;
    Vector ab_, ac_;
};
