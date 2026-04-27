#include "OBJLoader.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <limits>
#include <optional>
#include <stdexcept>
#include <string>
#include <sstream>

struct ObjIndex {
    int v = -1;
    int vt = -1;
    int vn = -1;
};

static int resolveObjIndex(int objIndex, int size)
{
    if (objIndex > 0) {
        const int idx = objIndex - 1;
        return (idx >= 0 && idx < size) ? idx : -1;
    }
    if (objIndex < 0) {
        const int idx = size + objIndex;
        return (idx >= 0 && idx < size) ? idx : -1;
    }
    return -1;
}

static bool parseIntStrict(const std::string &s, int &out)
{
    if (s.empty())
        return false;
    char *end = nullptr;
    long v = std::strtol(s.c_str(), &end, 10);
    if (end == s.c_str() || *end != '\0')
        return false;
    if (v < std::numeric_limits<int>::min() || v > std::numeric_limits<int>::max())
        return false;
    out = static_cast<int>(v);
    return true;
}

static bool parseFaceToken(
    const std::string &token,
    int vCount,
    int vtCount,
    int vnCount,
    ObjIndex &out)
{
    // token formats: v | v/vt | v//vn | v/vt/vn (indices are 1-based; can be negative)
    std::array<std::string, 3> parts;
    int partCount = 0;
    std::string cur;
    for (char c : token) {
        if (c == '/') {
            if (partCount < 3) {
                parts[partCount++] = cur;
            }
            cur.clear();
        } else {
            cur.push_back(c);
        }
    }
    if (partCount < 3)
        parts[partCount++] = cur;

    int vRaw = 0;
    if (!parseIntStrict(parts[0], vRaw))
        return false;

    int vtRaw = 0;
    int vnRaw = 0;
    const bool hasVT = (partCount >= 2 && !parts[1].empty());
    const bool hasVN = (partCount >= 3 && !parts[2].empty());
    if (hasVT && !parseIntStrict(parts[1], vtRaw))
        return false;
    if (hasVN && !parseIntStrict(parts[2], vnRaw))
        return false;

    out.v = resolveObjIndex(vRaw, vCount);
    out.vt = hasVT ? resolveObjIndex(vtRaw, vtCount) : -1;
    out.vn = hasVN ? resolveObjIndex(vnRaw, vnCount) : -1;
    return out.v != -1;
}

struct Vec2d {
    double x;
    double y;
};

static Vec3 newellNormal(const std::vector<Vec3> &poly)
{
    Vec3 n(0.0f);
    if (poly.size() < 3)
        return n;

    for (size_t i = 0; i < poly.size(); ++i) {
        const Vec3 &p0 = poly[i];
        const Vec3 &p1 = poly[(i + 1) % poly.size()];
        n.x += (p0.y - p1.y) * (p0.z + p1.z);
        n.y += (p0.z - p1.z) * (p0.x + p1.x);
        n.z += (p0.x - p1.x) * (p0.y + p1.y);
    }
    return n;
}

static std::vector<Vec2d> projectTo2D(const std::vector<Vec3> &poly)
{
    std::vector<Vec2d> out;
    out.reserve(poly.size());

    Vec3 n = newellNormal(poly);
    const double ax = std::fabs(static_cast<double>(n.x));
    const double ay = std::fabs(static_cast<double>(n.y));
    const double az = std::fabs(static_cast<double>(n.z));

    // Drop the dominant axis to reduce numerical issues.
    if (ax >= ay && ax >= az) {
        for (const Vec3 &p : poly)
            out.push_back({static_cast<double>(p.y), static_cast<double>(p.z)});
    } else if (ay >= az) {
        for (const Vec3 &p : poly)
            out.push_back({static_cast<double>(p.x), static_cast<double>(p.z)});
    } else {
        for (const Vec3 &p : poly)
            out.push_back({static_cast<double>(p.x), static_cast<double>(p.y)});
    }
    return out;
}

static double signedArea2D(const std::vector<Vec2d> &poly)
{
    double a = 0.0;
    for (size_t i = 0; i < poly.size(); ++i) {
        const Vec2d &p0 = poly[i];
        const Vec2d &p1 = poly[(i + 1) % poly.size()];
        a += p0.x * p1.y - p1.x * p0.y;
    }
    return a * 0.5;
}

static double cross2D(const Vec2d &a, const Vec2d &b, const Vec2d &c)
{
    // (b-a) x (c-a)
    return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

static bool pointInTri2D(const Vec2d &p, const Vec2d &a, const Vec2d &b, const Vec2d &c)
{
    // Barycentric technique with sign checks (includes points on edges).
    const auto sign = [](const Vec2d &p1, const Vec2d &p2, const Vec2d &p3) {
        return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
    };

    const double d1 = sign(p, a, b);
    const double d2 = sign(p, b, c);
    const double d3 = sign(p, c, a);

    const bool hasNeg = (d1 < 0.0) || (d2 < 0.0) || (d3 < 0.0);
    const bool hasPos = (d1 > 0.0) || (d2 > 0.0) || (d3 > 0.0);
    return !(hasNeg && hasPos);
}

static std::vector<std::array<int, 3>> triangulateEarClipping(const std::vector<Vec2d> &poly)
{
    std::vector<std::array<int, 3>> tris;
    const size_t n = poly.size();
    if (n < 3)
        return tris;

    std::vector<int> V;
    V.reserve(n);
    for (size_t i = 0; i < n; ++i)
        V.push_back(static_cast<int>(i));

    const double area = signedArea2D(poly);
    const bool isCCW = (area > 0.0);

    const double eps = 1e-12;
    const int maxIters = static_cast<int>(n) * static_cast<int>(n) + 10;
    int iter = 0;

    while (V.size() > 3 && iter++ < maxIters) {
        bool clipped = false;
        const size_t m = V.size();
        for (size_t i = 0; i < m; ++i) {
            const int i0 = V[(i + m - 1) % m];
            const int i1 = V[i];
            const int i2 = V[(i + 1) % m];

            const Vec2d &a = poly[i0];
            const Vec2d &b = poly[i1];
            const Vec2d &c = poly[i2];

            const double cr = cross2D(a, b, c);
            // Convexity depends on winding.
            if (isCCW) {
                if (cr <= eps)
                    continue; // reflex or nearly collinear
            } else {
                if (cr >= -eps)
                    continue; // reflex or nearly collinear
            }

            bool anyInside = false;
            for (size_t j = 0; j < m; ++j) {
                const int ij = V[j];
                if (ij == i0 || ij == i1 || ij == i2)
                    continue;
                if (pointInTri2D(poly[ij], a, b, c)) {
                    anyInside = true;
                    break;
                }
            }
            if (anyInside)
                continue;

            tris.push_back({i0, i1, i2});
            V.erase(V.begin() + static_cast<long>(i));
            clipped = true;
            break;
        }
        if (!clipped)
            break;
    }

    if (V.size() == 3) {
        tris.push_back({V[0], V[1], V[2]});
    }
    return tris;
}

static Vec3 faceNormal(const Vec3 &a, const Vec3 &b, const Vec3 &c)
{
    Vec3 n = cross(b - a, c - a);
    const float len2 = n.x * n.x + n.y * n.y + n.z * n.z;
    if (len2 <= 1e-20f)
        return Vec3(0.0f);
    return n.normalize();
}

static std::string dirnameOfPath(const std::string& path) {
    const size_t pos = path.find_last_of("/\\");
    if (pos == std::string::npos) return "";
    return path.substr(0, pos);
}

static std::string joinPath(const std::string& dir, const std::string& file) {
    if (dir.empty()) return file;
    if (file.empty()) return dir;
    const char last = dir.back();
    if (last == '/' || last == '\\') return dir + file;
    return dir + "/" + file;
}

static void scanObjForMaterialUsage(const std::string& objPath, std::string& outMtlFile, std::string& outUsedMaterial, bool& outMultipleUseMtl) {
    outMtlFile.clear();
    outUsedMaterial.clear();
    outMultipleUseMtl = false;

    std::ifstream file(objPath);
    if (!file.is_open())
        return;

    std::stringstream ss;
    std::string line;
    std::string prefix;

    while (std::getline(file, line)) {
        ss.clear();
        ss.str(line);
        ss >> prefix;

        if (prefix == "mtllib") {
            std::string mtl;
            ss >> mtl;
            if (outMtlFile.empty() && !mtl.empty()) {
                outMtlFile = mtl;
            }
        } else if (prefix == "usemtl") {
            std::string mat;
            ss >> mat;
            if (mat.empty())
                continue;
            if (outUsedMaterial.empty()) {
                outUsedMaterial = mat;
            } else if (outUsedMaterial != mat) {
                outMultipleUseMtl = true;
            }
        }
    }
}

static std::optional<ObjMaterialData> parseMtlMaterial(const std::string& mtlPath, const std::string& wantedName) {
    std::ifstream file(mtlPath);
    if (!file.is_open())
        return std::nullopt;

    ObjMaterialData out;
    out.name = wantedName;

    std::stringstream ss;
    std::string line;
    std::string prefix;
    bool inTarget = false;
    bool foundTarget = false;

    while (std::getline(file, line)) {
        ss.clear();
        ss.str(line);
        ss >> prefix;
        if (prefix.empty() || prefix == "#")
            continue;

        if (prefix == "newmtl") {
            std::string name;
            ss >> name;

            // If we were parsing the target and we encounter a new one, we can stop.
            if (inTarget && foundTarget)
                break;

            inTarget = (name == wantedName);
            if (inTarget)
                foundTarget = true;
            continue;
        }

        if (!inTarget)
            continue;

        if (prefix == "Ka") {
            ss >> out.Ka.x >> out.Ka.y >> out.Ka.z;
        } else if (prefix == "Kd") {
            ss >> out.Kd.x >> out.Kd.y >> out.Kd.z;
        } else if (prefix == "Ks") {
            ss >> out.Ks.x >> out.Ks.y >> out.Ks.z;
        } else if (prefix == "Ns") {
            ss >> out.Ns;
        } else if (prefix == "d") {
            ss >> out.d;
        } else if (prefix == "Tr") {
            float tr = 0.f;
            ss >> tr;
            out.d = 1.f - tr;
        } else if (prefix == "illum") {
            ss >> out.illum;
        }
    }

    if (!foundTarget)
        return std::nullopt;
    return out;
}

std::vector<Vertex> loadObj(const char *filePath) {

    // Vertex data
    std::vector<Vec3> vertex_positions;
    std::vector<Vec2> vertex_texcoords;
    std::vector<Vec3> vertex_normals;

    // Expanded triangle list
    std::vector<Vertex> verticies;

    std::stringstream ss;
    std::ifstream file(filePath);
    std::string line = "";
    std::string prefix = "";

    Vec3 temp_vec3;
    Vec2 temp_vec2;
    int faceCount = 0;

    if (!file.is_open()) {
        throw std::runtime_error("ERROR::OBJLOADER::Could not open file.");
    }

    while (std::getline(file, line)) {
        ss.clear();
        ss.str(line);
        ss >> prefix;

        if (prefix == "#") {
            continue;
        }
        else if (prefix == "o") {

        }
        else if (prefix == "s") {

        }
        else if (prefix == "usemtl") {
            // Material handling is done elsewhere (or ignored). Keep parsing geometry.
        }
        else if (prefix == "v") {
            ss >> temp_vec3.x >> temp_vec3.y >> temp_vec3.z;
            vertex_positions.push_back(temp_vec3);
        }
        else if (prefix == "vt") {
            ss >> temp_vec2.x >> temp_vec2.y;
            vertex_texcoords.push_back(temp_vec2);
        }
        else if (prefix == "vn") {
            ss >> temp_vec3.x >> temp_vec3.y >> temp_vec3.z;
            vertex_normals.push_back(temp_vec3);
        }
        else if (prefix == "f") {

            std::vector<ObjIndex> face;
            std::string tok;
            while (ss >> tok) {
                ObjIndex idx;
                if (!parseFaceToken(tok, static_cast<int>(vertex_positions.size()), static_cast<int>(vertex_texcoords.size()), static_cast<int>(vertex_normals.size()), idx)) {
                    face.clear();
                    break;
                }
                face.push_back(idx);
            }

            if (face.size() < 3) {
                faceCount++;
                continue;
            }

            std::vector<Vec3> poly3d;
            poly3d.reserve(face.size());
            for (const ObjIndex &idx : face)
                poly3d.push_back(vertex_positions.at(static_cast<size_t>(idx.v)));

            // Procedural UV fallback (used only when vt is missing).
            // We project the polygon to 2D and use those coordinates as UVs.
            // With GL_REPEAT this avoids per-triangle stretching.
            const std::vector<Vec2d> poly2d = projectTo2D(poly3d);
            constexpr float uvScale = 1.0f;
            std::vector<Vec2> faceUV;
            faceUV.reserve(poly2d.size());
            for (const Vec2d &p : poly2d)
                faceUV.push_back(Vec2(static_cast<float>(p.x) * uvScale, static_cast<float>(p.y) * uvScale));

            std::vector<std::array<int, 3>> tris;
            if (face.size() == 3) {
                tris.push_back({0, 1, 2});
            } else {
                tris = triangulateEarClipping(poly2d);
                if (tris.empty()) {
                    // Fallback: simple fan triangulation (can be wrong for concave polygons)
                    for (size_t i = 1; i + 1 < face.size(); ++i)
                        tris.push_back({0, static_cast<int>(i), static_cast<int>(i + 1)});
                }
            }

            const float color = fmod(static_cast<float>(faceCount) * 0.1f, 1.f) * 0.5f;
            const Vec3 faceColor(color);

            for (const auto &t : tris) {
                const ObjIndex &i0 = face.at(static_cast<size_t>(t[0]));
                const ObjIndex &i1 = face.at(static_cast<size_t>(t[1]));
                const ObjIndex &i2 = face.at(static_cast<size_t>(t[2]));

                const Vec3 &p0 = vertex_positions.at(static_cast<size_t>(i0.v));
                const Vec3 &p1 = vertex_positions.at(static_cast<size_t>(i1.v));
                const Vec3 &p2 = vertex_positions.at(static_cast<size_t>(i2.v));

                Vec3 n0(0.0f), n1(0.0f), n2(0.0f);
                if (i0.vn != -1 && i1.vn != -1 && i2.vn != -1) {
                    n0 = vertex_normals.at(static_cast<size_t>(i0.vn));
                    n1 = vertex_normals.at(static_cast<size_t>(i1.vn));
                    n2 = vertex_normals.at(static_cast<size_t>(i2.vn));
                } else {
                    const Vec3 fn = faceNormal(p0, p1, p2);
                    n0 = fn;
                    n1 = fn;
                    n2 = fn;
                }

                auto makeVertex = [&](const ObjIndex &idx, const Vec3 &pos, const Vec3 &nrm, const Vec2 &fallbackUV) {
                    Vertex ve;
                    ve.position = pos;
                    ve.color = faceColor;
                    ve.normal = nrm;
                    if (idx.vt != -1) {
                        ve.texCoords = vertex_texcoords.at(static_cast<size_t>(idx.vt));
                    } else {
                        ve.texCoords = fallbackUV;
                    }
                    verticies.push_back(ve);
                };

                // Use face-projected UVs when vt is missing; otherwise use OBJ vt.
                // Fallback to a deterministic per-triangle mapping only if projection is unusable.
                const bool hasFaceUV = (faceUV.size() == face.size());
                const Vec2 uv0 = hasFaceUV ? faceUV.at(static_cast<size_t>(t[0])) : Vec2(0.f, 0.f);
                const Vec2 uv1 = hasFaceUV ? faceUV.at(static_cast<size_t>(t[1])) : Vec2(1.f, 0.f);
                const Vec2 uv2 = hasFaceUV ? faceUV.at(static_cast<size_t>(t[2])) : Vec2(0.f, 1.f);

                makeVertex(i0, p0, n0, uv0);
                makeVertex(i1, p1, n1, uv1);
                makeVertex(i2, p2, n2, uv2);
            }

            faceCount++;
        }
    }
    
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

LoadedObjData loadObjWithMaterial(const char* filePath) {
    LoadedObjData out;
    out.vertices = loadObj(filePath);

    const std::string objPath(filePath ? filePath : "");
    if (objPath.empty())
        return out;

    std::string mtlFile;
    std::string usedMaterial;
    bool multipleUseMtl = false;
    scanObjForMaterialUsage(objPath, mtlFile, usedMaterial, multipleUseMtl);

    if (mtlFile.empty() || usedMaterial.empty())
        return out;

    const std::string mtlPath = joinPath(dirnameOfPath(objPath), mtlFile);
    const std::optional<ObjMaterialData> mat = parseMtlMaterial(mtlPath, usedMaterial);
    if (mat.has_value()) {
        out.material = mat;
    }

    (void)multipleUseMtl; // currently ignored (single-material mesh pipeline)
    return out;
}