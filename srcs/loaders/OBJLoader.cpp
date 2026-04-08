#include "OBJLoader.hpp"

#include <algorithm>
#include <iostream>
#include <fstream>
#include <limits>
#include <string>
#include <sstream>

std::vector<Vertex> loadObj(const char *filePath) {

    // Vertex
    std::vector<Vec3> vertex_positions;
    std::vector<Vec2> vertex_texcoords;
    std::vector<Vec3> vertex_normals;

    // Face
    std::vector<GLint> vertex_position_indicies;
    std::vector<GLint> vertex_texcoord_indicies;
    std::vector<GLint> vertex_normal_indicies;

    //Vertex array
    std::vector<Vertex> verticies;

    std::stringstream ss;
    std::ifstream file(filePath);
    std::string line = "";
    std::string prefix = "";

    Vec3 temp_vec3;
    Vec2 temp_vec2;
    // GLint temp_glint = 0;

    if (!file.is_open()) {
        throw "ERROR::OBJLOADER::Could not open file.";
    }

    while (std::getline(file, line)) {
        ss.clear();
        ss.str(line);
        ss >> prefix;

        if (prefix == "#") {

        }
        else if (prefix == "o") {

        }
        else if (prefix == "s") {

        }
        else if (prefix == "usemtl") {

        }
        else if (prefix == "v") {
            ss >> temp_vec3.x >> temp_vec3.y >> temp_vec3.z;
            vertex_positions.push_back(temp_vec3);
        }
        else if (prefix == "f") {
            std::vector<int> idx;
            int v = 0;
            while (ss >> v)
                idx.push_back(v);

            auto pushVertex = [&](int objIndex) {
                Vertex ve;
                ve.position = vertex_positions.at(objIndex - 1);
                ve.color = Vec3(1.f);
                ve.texCoords = Vec2();
                ve.normal = Vec3();
                verticies.push_back(ve);
            };

            if (idx.size() == 3) {
                pushVertex(idx.at(0));
                verticies.back().texCoords = Vec2(0.f, 0.f);
                pushVertex(idx.at(1));
                verticies.back().texCoords = Vec2(1.f, 0.f);
                pushVertex(idx.at(2));
                verticies.back().texCoords = Vec2(0.f, 1.f);
            } else if (idx.size() == 4) {
                pushVertex(idx.at(0));
                verticies.back().texCoords = Vec2(0.f, 0.f);
                pushVertex(idx.at(1));
                verticies.back().texCoords = Vec2(1.f, 0.f);
                pushVertex(idx.at(2));
                verticies.back().texCoords = Vec2(1.f, 1.f);

                pushVertex(idx.at(0));
                verticies.back().texCoords = Vec2(0.f, 0.f);
                pushVertex(idx.at(2));
                verticies.back().texCoords = Vec2(1.f, 1.f);
                pushVertex(idx.at(3));
                verticies.back().texCoords = Vec2(0.f, 1.f);
            }
        }
        else {

        }
    }

    // Recenter mesh around its bounding box center so that rotations happen around the center.
    // if (!vertex_positions.empty()) {
    //     Vec3 bboxMin(
    //         std::numeric_limits<float>::infinity(),
    //         std::numeric_limits<float>::infinity(),
    //         std::numeric_limits<float>::infinity());
    //     Vec3 bboxMax(
    //         -std::numeric_limits<float>::infinity(),
    //         -std::numeric_limits<float>::infinity(),
    //         -std::numeric_limits<float>::infinity());

    //     for (const Vec3 &p : vertex_positions) {
    //         bboxMin.x = std::min(bboxMin.x, p.x);
    //         bboxMin.y = std::min(bboxMin.y, p.y);
    //         bboxMin.z = std::min(bboxMin.z, p.z);
    //         bboxMax.x = std::max(bboxMax.x, p.x);
    //         bboxMax.y = std::max(bboxMax.y, p.y);
    //         bboxMax.z = std::max(bboxMax.z, p.z);
    //     }

    //     const Vec3 pivot(
    //         (bboxMin.x + bboxMax.x) * 0.5f,
    //         (bboxMin.y + bboxMax.y) * 0.5f,
    //         (bboxMin.z + bboxMax.z) * 0.5f);

    //     for (Vertex &ve : verticies) {
    //         ve.position = ve.position - pivot;
    //     }
    // }

    if (!verticies.empty()) {
        float minX = std::numeric_limits<float>::infinity();
        float maxX = -std::numeric_limits<float>::infinity();
        float minY = std::numeric_limits<float>::infinity();
        float maxY = -std::numeric_limits<float>::infinity();
        float minZ = std::numeric_limits<float>::infinity();
        float maxZ = -std::numeric_limits<float>::infinity();

        for (auto &v : verticies) {
            minX = std::min(minX, v.position.x);
            maxX = std::max(maxX, v.position.x);

            minY = std::min(minY, v.position.y);
            maxY = std::max(maxY, v.position.y);

            minZ = std::min(minZ, v.position.z);
            maxZ = std::max(maxZ, v.position.z);
        }
        float centerX = (minX + maxX) / 2;
        float centerY = (minY + maxY) / 2;
        float centerZ = (minZ + maxZ) / 2;

        for (auto &v : verticies) {
            v.position.x -= centerX;
            v.position.y -= centerY;
            v.position.z -= centerZ;
        }
    }

    return verticies;
}