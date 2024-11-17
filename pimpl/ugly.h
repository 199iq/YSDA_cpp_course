#pragma once

#include <vector>
#include <memory>

class Spline {
public:
    // Build a spline through points {(x[0], y[0]), (x[1], y[1]), ...}.
    Spline(std::vector<double> x, std::vector<double> y, double a = 0., double b = 0.);
    ~Spline();

    // Get spline value at a given point.
    double Interpolate(double x) const;

private:
    class SplineImpl;
    std::unique_ptr<SplineImpl> impl_;
};
