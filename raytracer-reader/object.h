#pragma once

#include "triangle.h"
#include "material.h"
#include "sphere.h"
#include "vector.h"
#include <vector>

struct Object {
    const Material* material = nullptr;
    Triangle polygon;
    std::vector<Vector*> normals = {nullptr, nullptr, nullptr};
    
    explicit Object(Triangle polygon)
        : polygon(polygon) {
    }

    const Vector* GetNormal(size_t index) const {
        return normals[index];
    }
};

struct SphereObject {
    const Material* material = nullptr;
    Sphere sphere;
};
