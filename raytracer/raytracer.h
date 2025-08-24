#pragma once

#include "options/camera_options.h"
#include "options/render_options.h"
#include "../raytracer-reader/scene.h" // this connects geometry modules
#include "image.h"

#include <filesystem>
#include <iostream>
#include <vector>

// ---------------------- Constants ----------------------

const double INF = 1e9;
const double kEps = 1e-4;

// ---------------------- Intersections & RayTracing ----------------------

std::optional<Intersection>
GetSmoothIntersection(const Ray& ray, const Object& obj) {
    auto naive_intersection = GetIntersection(ray, obj.polygon);

    if (naive_intersection == std::nullopt) {
        return naive_intersection;
    }

    for (size_t i = 0; i < obj.normals.size(); i++) {
        if (obj.normals[i] == nullptr) {
            return naive_intersection;
        }
    }

    Vector coeff = GetBarycentricCoords(obj.polygon, naive_intersection.value().GetPosition());
    Vector smooth_normal = coeff[0] * (*obj.normals[0]) + coeff[1] * (*obj.normals[1]) + coeff[2] * (*obj.normals[2]);
    smooth_normal.Normalize();

    return Intersection{naive_intersection.value().GetPosition(), smooth_normal, naive_intersection.value().GetDistance()};
}

std::pair<std::optional<Intersection>, std::pair<const Material*, std::string>>
GetClosestIntersection(Ray ray, const Scene& scene) {
    double mn_dist = INF;
    const Material* material = nullptr;
    std::optional<Intersection> ans_intersection = std::nullopt;
    std::string name = "";

    // try objects
    for (const auto& obj: scene.GetObjects()) {
        std::optional<Intersection> intersection = GetSmoothIntersection(ray, obj);
        if (intersection == std::nullopt) {
            continue;
        }

        if (intersection.value().GetDistance() < mn_dist) {
            mn_dist = intersection.value().GetDistance();
            material = obj.material;
            ans_intersection = intersection;
            name = "triangle";
        }
    }
    // try sphere objects
    for (const auto& obj: scene.GetSphereObjects()) {
        std::optional<Intersection> intersection = GetIntersection(ray, obj.sphere);
        if (intersection == std::nullopt) {
            continue;
        }
        if (intersection.value().GetDistance() < mn_dist) {
            mn_dist = intersection.value().GetDistance();
            material = obj.material;
            ans_intersection = intersection;
            name = "sphere";
        }
    }

    return {ans_intersection, {material, name}};
}

bool CanGoThrough(const Material* material) {
    return material->refraction_index == 1 && material->albedo[2] != 0;
}

Vector ComputeDirectLight(const Scene& scene, const Light& light, const Vector& pos, int iters_left) {
    if (iters_left < 0) {
        return {0, 0, 0};
    }

    Vector ray_dir = pos - light.position;
    ray_dir.Normalize();
    Ray ray = Ray(light.position, ray_dir);

    auto [intersection, object] = GetClosestIntersection(ray, scene);
    auto material = object.first;

    if (intersection == std::nullopt) {
        return {0, 0, 0};
    }

    if (Length(intersection.value().GetPosition() - pos) < kEps) {
        return light.intensity;
    }

    if (CanGoThrough(material)) {
        Vector new_pos = intersection.value().GetPosition();
        new_pos += kEps * ray_dir;
        // new_pos += kEps * intersection.value().GetNormal(); // this works too (maybe even better)

        Light new_light = Light(new_pos, light.intensity * material->albedo[2]);
        return ComputeDirectLight(scene, new_light, pos, iters_left - 1);
    }
    
    return {0, 0, 0};
}

Ray OffsetRay(Vector origin, Vector dir) {
    return Ray(origin + dir * kEps, dir);
}

Vector ComputeDiffuse(const Scene& scene, const Intersection& intersection, int iters_left) {
    Vector illum_diffuse = {0, 0, 0};
    for (const Light& light: scene.GetLights()) {
        Vector light_dir = (light.position - intersection.GetPosition());
        light_dir.Normalize();

        Vector light_intensity = ComputeDirectLight(scene, light, intersection.GetPosition(), iters_left);

        illum_diffuse += light_intensity * std::max(0.0, DotProduct(intersection.GetNormal(), light_dir));
    }

    return illum_diffuse;
}

Vector ComputeSpecular(const Scene& scene, const Intersection& intersection,
                       const Material* material, const Ray& ray, int iters_left) {
    Vector illum_specular = {0, 0, 0};
    for (const Light& light: scene.GetLights()) {
        Vector light_intensity = ComputeDirectLight(scene, light, intersection.GetPosition(), iters_left);

        Vector light_dir = (light.position - intersection.GetPosition());
        light_dir.Normalize();

        // same as Reflect(-light_dir, intersection.GetNormal()) ?
        Vector reflect_dir = 2.0 * DotProduct(intersection.GetNormal(), light_dir) * intersection.GetNormal() - light_dir;

        double cos_sigma = -DotProduct(ray.GetDirection(), reflect_dir);
        illum_specular += light_intensity * pow(std::max(0.0, cos_sigma), material->specular_exponent);
    }
    return illum_specular;
}

Vector ComputeIllumination(const Scene& scene, const Ray& ray, bool inside, int iters_left) {
    if (iters_left < 0) {
        return {0, 0, 0};
    }

    auto [poss_intersection, object] = GetClosestIntersection(ray, scene);
    if (poss_intersection == std::nullopt) {
        return {0, 0, 0};
    }

    auto intersection = poss_intersection.value();
    auto [material, name] = object;

    // Ambient
    Vector illum_ambient = material->ambient_color + material->intensity;

    // Diffuse
    Vector illum_diffuse = material->diffuse_color * ComputeDiffuse(scene, intersection, iters_left - 1);

    // Specular
    Vector illum_specular = material->specular_color * ComputeSpecular(scene, intersection, material, ray, iters_left - 1);

    // Reflected
    Vector illum_reflected = {0, 0, 0};
    if (!inside && material->albedo[1]) {
        Vector reflected_ray_dir = Reflect(ray.GetDirection(), intersection.GetNormal());
        reflected_ray_dir.Normalize();

        Ray reflected_ray = OffsetRay(intersection.GetPosition(), reflected_ray_dir);
        illum_reflected = material->albedo[1] * ComputeIllumination(scene, reflected_ray, inside, iters_left - 1); // * material->specular_color;
    }

    // Refracted
    // do we need to distinguish triangles from spheres ?
    // so we could change INSIDE flag for them accordingly
    std::optional<Vector> refracted_ray_dir = std::nullopt;
    Vector illum_refracted = {0, 0, 0};
    if (inside) {
        refracted_ray_dir = Refract(ray.GetDirection(), intersection.GetNormal(), material->refraction_index);
    } else {
        refracted_ray_dir = Refract(ray.GetDirection(), intersection.GetNormal(), 1.0 / material->refraction_index);
    }

    if (refracted_ray_dir != std::nullopt && material->albedo[2]) {
        Ray refracted_ray = OffsetRay(intersection.GetPosition(), refracted_ray_dir.value());
        if (name == "triangle") {
            illum_refracted = material->albedo[2] * ComputeIllumination(scene, refracted_ray, inside, iters_left - 1);
        } else {
            illum_refracted = material->albedo[2] * ComputeIllumination(scene, refracted_ray, !inside, iters_left - 1);
        }
    }

    if (inside && material->albedo[2]) {
        illum_refracted *= (material->albedo[1] + material->albedo[2]) / material->albedo[2];
    }

    return illum_ambient + material->albedo[0] * (illum_diffuse + illum_specular) + illum_reflected + illum_refracted;
}

void PostProcess(std::vector<std::vector<Vector>>& image) {
    double mx = 0;
    for (size_t row = 0; row < image.size(); row++) {
        for (size_t col = 0; col < image[0].size(); col++) {
            for (size_t idx = 0; idx < 3; idx++) {
                mx = std::max(mx, image[row][col][idx]);
            }
        }
    }

    // tone mapping & gamma correction
    for (size_t row = 0; row < image.size(); row++) {
        for (size_t col = 0; col < image[0].size(); col++) {
            for (size_t idx = 0; idx < 3; idx++) {
                // tone mapping
                image[row][col][idx] *= (1 + image[row][col][idx] / (mx * mx)) / (1 + image[row][col][idx]);
                // gamma correction
                image[row][col][idx] = pow(image[row][col][idx], 1 / 2.2);
            }
        }
    }
}

// ---------------------- Ray casting ----------------------

class RayCaster {
public:
    constexpr static double raycaster_eps = 1e-4;

    explicit RayCaster(const CameraOptions& camera_options) {
        origin_ = camera_options.look_from;
        screen_height_ = camera_options.screen_height;
        screen_width_ = camera_options.screen_width;

        Z_ = camera_options.look_from - camera_options.look_to;
        Z_.Normalize();

        X_ = CrossProduct({0, 1, 0}, Z_);
        if (Length(X_) < raycaster_eps) {
            // Y = {0, 1, 0} is almost collinear to Z_
            X_ = {1, 0, 0};
            Y_ = CrossProduct(X_, Z_);
            Y_.Normalize();
        } else {
            X_.Normalize();
            Y_ = CrossProduct(X_, Z_);
            Y_.Normalize();
        }

        // fov = fov_vertical; fov_horizontal updates automatically ? 
        double pixel_size = 2 * tan(camera_options.fov / 2) / camera_options.screen_height;
        pixel_size_ = pixel_size;

        // update basis
        X_ = X_ * pixel_size;
        Y_ = Y_ * pixel_size;
    }

    Ray operator() (int idx_x, int idx_y) const {
        Vector direction =
                static_cast<double>((2.0 * idx_x - screen_width_ + 1.0) / 2.0) * X_ +
                static_cast<double>((2.0 * idx_y - screen_height_ + 1.0) / 2.0) * Y_ +
                (-Z_);
        direction.Normalize();
        return Ray(origin_, direction);
    }
private:
    int screen_height_, screen_width_;
    Vector origin_, X_, Y_, Z_;
    double pixel_size_;
};

// ---------------------- Rendering ----------------------

Image RenderDepth(const Scene& scene, const CameraOptions& camera_options, const RayCaster& raycaster) {

    Image image = Image(camera_options.screen_width, camera_options.screen_height);
    std::vector<std::vector<double>> dist(image.Height(), std::vector<double>(image.Width()));
    double mx_dist = 0;

    for (int row = 0; row < image.Height(); row++) {
        for (int col = 0; col < image.Width(); col++) {
            Ray ray = raycaster(col, row);
            auto intersection = GetClosestIntersection(ray, scene).first;
            if (intersection != std::nullopt) {
                dist[row][col] = intersection.value().GetDistance();
                mx_dist = std::max(mx_dist, dist[row][col]);
            }
        }
    }

    int mx_color = 0;

    for (int row = 0; row < image.Height(); row++) {
        for (int col = 0; col < image.Width(); col++) {
            int color = static_cast<int>(((dist[row][col] ? dist[row][col] / mx_dist : 1.) - kEps) * 256);
            mx_color = std::max(mx_color, color);
            image.SetPixel(row, col, {color, color, color});
        }
    }
    return image;
}

Image RenderNormal(const Scene& scene, const CameraOptions& camera_options, const RayCaster& raycaster) {

    Image image = Image(camera_options.screen_width, camera_options.screen_height);

    for (int row = 0; row < image.Height(); row++) {
        for (int col = 0; col < image.Width(); col++) {
            Ray ray = raycaster(col, row);
            auto intersection = GetClosestIntersection(ray, scene).first;
            Vector pixel = {0, 0, 0};

            if (intersection != std::nullopt) {
                pixel = 0.5 * intersection.value().GetNormal() + Vector{0.5, 0.5, 0.5};
            }
            int red = static_cast<int>((pixel[0] - kEps) * 256);
            int green = static_cast<int>((pixel[1] - kEps) * 256);
            int blue = static_cast<int>((pixel[2] - kEps) * 256);

            image.SetPixel(row, col, {red, green, blue});
        }
    }
    return image;
}

void PrintVector(std::string name, Vector vec) {
    std::cout << name << " " << vec[0] << " " << vec[1] << " " << vec[2] << "\n";
}

Image RenderFull(const Scene& scene, const CameraOptions& camera_options, const RayCaster& raycaster, int depth) {

    // debug
    // for (const auto& material: scene.GetMaterials()) {
    //     std::cout << "Name: " << material.first << "\n";
    //     PrintVector("ambient=", material.second.ambient_color);
    //     PrintVector("diffuse=", material.second.diffuse_color);
    //     PrintVector("specular=", material.second.specular_color);
    //     PrintVector("albedo=", material.second.albedo);
    //     std::cout << "refraction=" << material.second.refraction_index << "\n";
    //     PrintVector("intensity=", material.second.intensity);
    //     std::cout << "----------------------------------\n";
    // }

    Image image = Image(camera_options.screen_width, camera_options.screen_height);
    std::vector<std::vector<Vector>> pixels(image.Height(), std::vector<Vector>(image.Width()));

    for (int row = 0; row < image.Height(); row++) {
        for (int col = 0; col < image.Width(); col++) {
            Ray ray = raycaster(col, row);
            pixels[row][col] = ComputeIllumination(scene, ray, false, depth);
        }
    }

    PostProcess(pixels);

    for (int row = 0; row < image.Height(); row++) {
        for (int col = 0; col < image.Width(); col++) {
            Vector pixel = pixels[row][col];

            int red = static_cast<int>((pixel[0] - kEps) * 256);
            int green = static_cast<int>((pixel[1] - kEps) * 256);
            int blue = static_cast<int>((pixel[2] - kEps) * 256);

            image.SetPixel(row, col, {red, green, blue});
        }
    }
    return image;
}


Image Render(const std::filesystem::path& path, const CameraOptions& camera_options,
             const RenderOptions& render_options) {
    
    Scene scene = ReadScene(path);
    RayCaster raycaster = RayCaster(camera_options);
    
    if (render_options.mode == RenderMode::kDepth) {
        return RenderDepth(scene, camera_options, raycaster);
    } else if (render_options.mode == RenderMode::kNormal) {
        return RenderNormal(scene, camera_options, raycaster);
    } else if (render_options.mode == RenderMode::kFull) {
        return RenderFull(scene, camera_options, raycaster, render_options.depth);
    }
    
    throw std::runtime_error{"Wrong render mode"};
}
