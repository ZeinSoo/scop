#pragma once

#include <optional>
#include <string>
#include <vector>

#include "maths.hpp"

std::vector<Vertex> loadObj(const char *filePath);

struct ObjMaterialData {
	std::string name;
	Vec3 Ka = Vec3(0.f);
	Vec3 Kd = Vec3(0.64f);
	Vec3 Ks = Vec3(0.5f);
	float Ns = 96.078431f;
	float d = 1.f;
	int illum = 2;
};

struct LoadedObjData {
	std::vector<Vertex> vertices;
	std::optional<ObjMaterialData> material;
};

LoadedObjData loadObjWithMaterial(const char* filePath);
