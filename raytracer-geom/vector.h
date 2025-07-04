#pragma once

#include <array>
#include <cstddef>
#include <iostream>
#include <cmath>

class Vector {
public:
    Vector()
        : data_({0, 0, 0}) {
    }

    Vector(double x, double y, double z) {
        data_[0] = x;
        data_[1] = y;
        data_[2] = z;
    }

    double& operator[](size_t ind) {
        if (ind >= 3) {
            throw std::out_of_range("Index out of range");
        }
        return data_[ind];
    }

    double operator[](size_t ind) const {
        if (ind >= 3) {
            throw std::out_of_range("Index out of range");
        }
        return data_[ind];
    }

    Vector operator-() const {
        return Vector(-data_[0], -data_[1], -data_[2]);
    }

    Vector operator+(const Vector& rhs) const {
        return Vector(data_[0] + rhs[0], data_[1] + rhs[1], data_[2] + rhs[2]);
    }

    Vector operator-(const Vector& rhs) const {
        return *this + (-rhs);
    }

    Vector operator*(double scal) const {
        return Vector(data_[0] * scal, data_[1] * scal, data_[2] * scal);
    }

    double GetNorm() const {
        double res = 0;
        for (size_t i = 0; i < 3; i++) {
            res += data_[i] * data_[i];
        }
        return std::sqrt(res);
    }

    void Normalize() {
        double norm = GetNorm(); // this->GetNorm()
        for (size_t i = 0; i < 3; i++) {
            data_[i] /= norm;
        }
    }

private:
    std::array<double, 3> data_;
};

Vector operator*(double scal, const Vector& vec) {
    return Vector(scal * vec[0], scal * vec[1], scal * vec[2]);
}

double DotProduct(const Vector& a, const Vector& b) {
    double ans = 0;
    for (size_t i = 0; i < 3; i++) {
        ans += a[i] * b[i];
    }
    return ans;
}
Vector CrossProduct(const Vector& a, const Vector& b) {
    double new_x = a[1] * b[2] - a[2] * b[1];
    double new_y = a[2] * b[0] - a[0] * b[2];
    double new_z = a[0] * b[1] - a[1] * b[0];
    return Vector(new_x, new_y, new_z);
}

double Length(const Vector& v) {
    return v.GetNorm();
}

double MixedProduct(const Vector& a, const Vector& b, const Vector& c) {
    return DotProduct(a, CrossProduct(b, c));
}

Vector GetVector(const Vector& pt_x, const Vector& pt_y) {
    // return pt_y - pt_x;
    return Vector(pt_y[0] - pt_x[0], pt_y[1] - pt_x[1], pt_y[2] - pt_x[2]);
}
