#pragma once

#include "vector.h"
#include "sphere.h"
#include "intersection.h"
#include "triangle.h"
#include "ray.h"

#include <optional>
#include <utility>
#include <iostream>
#include <cassert>

constexpr double eps = 1e-10; // mb smhw fix that later

std::optional<Intersection> GetIntersection(const Ray& ray, const Sphere& sphere) {
    std::optional<Intersection> ans = std::nullopt;

    Vector dist = ray.GetOrigin() - sphere.GetCenter();
    double a = DotProduct(ray.GetDirection(), ray.GetDirection()); // should be always 1
    double b = 2.0 * DotProduct(ray.GetDirection(), dist);
    double c = DotProduct(dist, dist) - sphere.GetRadius() * sphere.GetRadius();

    double disc = b * b - 4.0 * a * c;
    double t = 0;

    if (disc < 0) {
        return ans; // no intersection
    } else if (disc == 0) {
        t = -b / (2.0 * a);
        if (t < 0) {
            return ans; // idk if that even happens
        }
    } else {
        t = (-b - std::sqrt(disc)) / (2.0 * a);
        if (t < 0) {
            t = (-b + std::sqrt(disc)) / (2.0 * a);
            if (t < 0) {
                return ans; // idk if that even happens
            }
        }
    }

    Vector point = ray.GetOrigin() + t * ray.GetDirection();
    Vector point_normal = point - sphere.GetCenter();
    point_normal.Normalize();

    if (DotProduct(ray.GetDirection(), point_normal) > 0) {
        point_normal = -point_normal;
    }

    ans = Intersection(point, point_normal, Length(point - ray.GetOrigin()));
    return ans;
}

std::optional<Intersection> GetIntersection(const Ray& ray, const Triangle& triangle) {
    std::optional<Intersection> ans = std::nullopt;
    std::pair<Vector, Vector> sides = triangle.GetSides();

    Vector neg_dir = -ray.GetDirection();
    Vector rhs = ray.GetOrigin() - triangle[0];
    double det = MixedProduct(sides.first, sides.second, neg_dir);

    if (det > -eps && det < eps) {
        // parallel case
        // std::cout << "WARNING: The equation can't be solved\n";
        return ans;
    }

    double inv_det = 1.0 / det;
    double det_x = MixedProduct(rhs, sides.second, neg_dir);
    double u = det_x * inv_det;

    if (u < 0.0 || u > 1.0) {
        // no intersection inside the triangle
        // std::cout << "WARNING: coeff u is bad " << u << "\n";
        return ans;
    }

    double det_y = MixedProduct(sides.first, rhs, neg_dir);
    double v = det_y * inv_det;

    if (v < 0.0 || u + v > 1.0) {
        // no intersection inside the triangle
        // std::cout << "WARNING: coeff v is bad " << u << " " << v << "\n";
        return ans;
    }

    // now we can compute t
    double det_z = MixedProduct(sides.first, sides.second, rhs);
    double t = det_z * inv_det;

    if (t > eps) {
        Vector point = ray.GetOrigin() + t * ray.GetDirection();
        Vector normal = triangle.GetNormal();
        if (DotProduct(normal, ray.GetDirection()) > 0) {
            normal = -normal;
        }

        ans = Intersection(point, normal, Length(point - ray.GetOrigin()));
    } else {
        // std::cout << "WARNING: t <= eps\n";
    }
    return ans;
}

Vector Reflect(const Vector& ray, const Vector& normal) {
    assert(fabs(Length(normal) - 1.0) < eps);

    return ray - 2.0 * DotProduct(ray, normal) * normal;
}

std::optional<Vector> Refract(const Vector& ray, const Vector& normal, double eta) {
    assert(fabs(Length(ray) - 1.0) < eps); // make sure that Ray is normalized
    assert(fabs(Length(normal) - 1.0) < eps);

    std::optional<Vector> ans = std::nullopt;
    double scal = -DotProduct(ray, normal);
    double tmp = 1.0 - eta * eta * (1.0 - scal * scal);

    if (tmp < 0) {
        return ans; // ray is reflected
    }

    ans = normal * (eta * scal - std::sqrt(tmp)) + eta * ray;
    return ans;
}

Vector GetBarycentricCoords(const Triangle& triangle, const Vector& point) {
    double s_a = Triangle(point, triangle[1], triangle[2]).Area(); // S_pbc
    double s_b = Triangle(point, triangle[2], triangle[0]).Area(); // S_pca
    double s_c = Triangle(point, triangle[0], triangle[1]).Area(); // S_pab
    double s = triangle.Area(); // S_abc

    if (s_a < 0 || s_b < 0 || s_c < 0) {
        throw std::runtime_error("The point is not in the triangle");
    }

    return Vector(s_a / s, s_b / s, s_c / s);
}
