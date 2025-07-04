#pragma once

#include "material.h"
// #include "vector.h"
#include "object.h"
#include "light.h"
#include "../raytracer-geom/geometry.h" // maybe not the best way to connect a module

#include <vector>
#include <unordered_map>
#include <string>
#include <filesystem>
#include <fstream>
#include <cassert>
#include <memory>

class Scene {
public:
    // maybe use later
    std::unordered_map<std::string, Material> RestoreMaterials(const std::unordered_map<std::string,
            std::unique_ptr<Material>>& material_pointers) {
        std::unordered_map<std::string, Material> materials;
        for (const auto& [name, pointer] : material_pointers) {
            materials[name] = *pointer;
        }
        return materials;
    }

    Scene(const std::vector<Object> objects,
        const std::vector<SphereObject> sphere_objects,
        const std::vector<Light> lights,
        std::unordered_map<std::string, std::unique_ptr<Material>>&& materials,
        std::vector<std::unique_ptr<Vector>>&& normal_pointers)

        : objects_(objects), sphere_objects_(sphere_objects), lights_(lights),
         material_pointers_(std::move(materials)), normal_pointers_(std::move(normal_pointers)) {
            // restore materials
            materials_ = RestoreMaterials(material_pointers_);
    }

    const std::vector<Object>& GetObjects() const {
        return objects_;
    }

    const std::vector<SphereObject>& GetSphereObjects() const {
        return sphere_objects_;
    }

    const std::vector<Light>& GetLights() const {
        return lights_;
    }

    const std::unordered_map<std::string, Material>& GetMaterials() const {
        return materials_;
    }
private:
    std::vector<Object> objects_;
    std::vector<SphereObject> sphere_objects_;
    std::vector<Light> lights_;
    std::unordered_map<std::string, std::unique_ptr<Material>> material_pointers_;
    std::vector<std::unique_ptr<Vector>> normal_pointers_;
    std::unordered_map<std::string, Material> materials_;
};

std::string GetFolderByFilename(const std::string& filename) {
    char sep = '/';

    size_t last_idx = filename.rfind(sep, filename.size());
    if (last_idx != std::string::npos) {
        return filename.substr(0, last_idx);
    }
    std::cout << "WARNING! No folder name extracted from given filename\n";
    return "";
}

std::vector<std::string> ParseLine(const std::string& line) {
    std::istringstream input(line);
    std::vector<std::string> res;

    std::string st;
    while (input >> st) {
        res.push_back(st);
    }
    return res;
}

Vector GetPoint(const std::vector<std::string>& line) {
    assert(line.size() == 4);

    Vector node = Vector(std::stod(line[1]), std::stod(line[2]), std::stod(line[3]));
    return node;
}

std::pair<int, int> GetTriplet(const std::string& line) {
    // returns {index of point, index of normal}
    std::istringstream input(line);
    std::string st;

    std::vector<int> tmp;

    while (std::getline(input, st, '/')) {
        if (st.empty()) {
            tmp.push_back(0);
        } else {
            int idx = static_cast<int>(std::stod(st));
            tmp.push_back(idx);
        }
    }
    if (tmp.size() < 3) {
        // there's just index of the point (no normal)
        return {tmp[0], 0};
    } else {
        // point & normal
        assert(!tmp[1]);
        return {tmp[0], tmp[2]};
    }
}

SphereObject GetSphere(const std::vector<std::string>& line, const Material* material) {
    assert(line[0] == "S");

    Vector center = Vector(std::stod(line[1]), std::stod(line[2]), std::stod(line[3]));
    double radius = std::stod(line[4]);

    return SphereObject(material, Sphere(center, radius));
}

Light GetLight(const std::vector<std::string>& line) {
    assert(line[0] == "P");

    Vector position = Vector(std::stod(line[1]), std::stod(line[2]), std::stod(line[3]));
    Vector intensity = Vector(std::stod(line[4]), std::stod(line[5]), std::stod(line[6]));

    return Light(position, intensity);
}

size_t GetIndex(size_t len, int idx) {
    if (!idx) {
        throw std::runtime_error("Idx of is equal to zero\n");
    }
    if (idx > 0) {
        return static_cast<size_t>(idx - 1);
    }
    if (len < abs(idx)) {
        throw std::runtime_error("Index is greater than current vector length\n");
    }
    return static_cast<size_t>(len + idx);
}

void GetFigure(const std::vector<std::string>& line,
               const std::vector<Vector>& nodes,
               const Material* material, std::vector<Object>& all_objects,
               std::vector<std::unique_ptr<Vector>>& normal_pointers) {

    assert(line[0] == "f");

    // DON'T FORGET TO PROCESS NEGATIVE INDICIES

    size_t num_points = line.size();
    std::vector<std::pair<int, int>> points;
    for (size_t i = 1; i < num_points; i++) {
        points.push_back(GetTriplet(line[i]));
    }

    num_points = points.size();

    assert(num_points >= 3);

    for (size_t i = 1; i + 1 < num_points; i++) {
        size_t idx_a = GetIndex(nodes.size(), points[0].first);
        size_t idx_b = GetIndex(nodes.size(), points[i].first);
        size_t idx_c = GetIndex(nodes.size(), points[i + 1].first);

        Triangle polygon = Triangle(nodes[idx_a], nodes[idx_b], nodes[idx_c]);
        std::vector<Vector*> obj_normals = {nullptr, nullptr, nullptr};
        std::vector<size_t> idx = {0, i, i + 1};

        for (int j = 0; j < 3; j++) {
            int cur_idx = points[idx[j]].second;
            obj_normals[j] = (cur_idx ? normal_pointers[GetIndex(normal_pointers.size(), cur_idx)].get() : nullptr);
        }

        Object new_obj = Object(polygon);
        new_obj.material = material;
        new_obj.normals = obj_normals;

        all_objects.push_back(new_obj);
    }
    return;
}

std::unordered_map<std::string, std::unique_ptr<Material>> ReadMaterials(const std::string& filename) {
    std::ifstream input(filename);
    std::string line;

    std::unordered_map<std::string, std::unique_ptr<Material>> materials;
    Material cur_material;
    bool real_material = false;

    while (std::getline(input, line)) {
        std::vector<std::string> parsed_line = ParseLine(line);
        if (parsed_line.empty()) {
            continue;
        }

        if (parsed_line[0][0] == '#') {
            // comment -> ignore
            continue;
        }

        if (parsed_line[0] == "newmtl") {
            if (real_material) {
                materials[cur_material.name] = std::make_unique<Material>(cur_material);
            }
            cur_material = Material();
            cur_material.name = parsed_line[1];
            real_material = true;
        } else if (parsed_line[0] == "Ks") {
            cur_material.specular_color = GetPoint(parsed_line);
        } else if (parsed_line[0] == "Kd") {
            cur_material.diffuse_color = GetPoint(parsed_line);
        } else if (parsed_line[0] == "Ka") {
            cur_material.ambient_color = GetPoint(parsed_line);
        } else if (parsed_line[0] == "Ke") {
            cur_material.intensity = GetPoint(parsed_line);
        } else if (parsed_line[0] == "al") {
            cur_material.albedo = GetPoint(parsed_line);
        } else if (parsed_line[0] == "Ni") {
            cur_material.refraction_index = std::stod(parsed_line[1]);
        } else if (parsed_line[0] == "Ns") {
            cur_material.specular_exponent = std::stod(parsed_line[1]);
        } else {
            // non-existing keyword
            continue;
        }
    }
    // add last material
    materials[cur_material.name] = std::make_unique<Material>(cur_material);
    return materials;
}

Scene ProcessScene(const std::string &filename) {
    // Scene fields
    std::vector<Object> objects_;
    std::vector<SphereObject> sphere_objects_;
    std::vector<Light> lights_;
    std::unordered_map<std::string, std::unique_ptr<Material>> materials_;
    std::vector<std::unique_ptr<Vector>> normal_pointers_;

    // Additional fields
    std::vector<Vector> nodes;
    std::vector<std::vector<int>> object_normals_idx;

    std::string folder = GetFolderByFilename(filename);
    std::ifstream input(filename);

    if (!input) {
        throw std::runtime_error("File was not found\n");
    }

    std::string line;
    Material* cur_material = nullptr;

    while (std::getline(input, line)) {
        std::vector<std::string> parsed_line = ParseLine(line);
        if (parsed_line.empty()) {
            // empty string
            continue;
        }
        if (parsed_line[0][0] == '#') {
            // comment -> ignore
            continue;
        }

        if (parsed_line[0] == "usemtl") {
            // new material
            cur_material = (materials_.find(parsed_line[1]) != materials_.end() ? materials_.at(parsed_line[1]).get() : nullptr);
        } else if (parsed_line[0] == "mtllib") {
            // process materials
            std::string material_filename = folder + "/" + parsed_line[1];
            materials_ = ReadMaterials(material_filename);
        } else if (parsed_line[0] == "v") {
            // node
            nodes.push_back(GetPoint(parsed_line));
        } else if (parsed_line[0] == "vt") {
            // texture node
            continue;
        } else if (parsed_line[0] == "vn") {
            // normal
            Vector cur_normal = GetPoint(parsed_line);
            normal_pointers_.push_back(std::make_unique<Vector>(cur_normal));
        } else if (parsed_line[0] == "f") {
            // figure
            GetFigure(parsed_line, nodes, cur_material, objects_, normal_pointers_);
        } else if (parsed_line[0] == "S") {
            // sphere
            sphere_objects_.push_back(GetSphere(parsed_line, cur_material));
        } else if (parsed_line[0] == "P") {
            // light
            lights_.push_back(GetLight(parsed_line));
        } else {
            // non-existing keyword
            continue;
        }
    }
    return Scene(objects_,
                sphere_objects_,
                lights_,
                std::move(materials_),
                std::move(normal_pointers_)
            );
}

Scene ReadScene(const std::filesystem::path& path) {
    std::string filename = path;
    return ProcessScene(filename);
}

/*
Notes:
-CHECK stod -> int
-CHECK normals argument in GetFigure function


*/
