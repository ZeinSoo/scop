#include "Engine.hpp"
#include "Logger.hpp"

#include <algorithm>

static std::string basenameOfPath(const std::string& path) {
    const size_t pos = path.find_last_of("/\\");
    if (pos == std::string::npos) return path;
    return path.substr(pos + 1);
}

void Engine::addTextureFromFile(const std::string& filePath) {
    if (filePath.empty()) {
        Logger::log("[Engine] addTextureFromFile: empty path");
        return;
    }

    if (this->findTextureByName(filePath) != nullptr) {
        Logger::log(std::string("[Engine] Texture already loaded: ") + filePath);
        return;
    }

    Texture* tex = new Texture(filePath.c_str(), GL_TEXTURE_2D);
    if (!tex->getID()){
        Logger::log(std::string("[Engine] Failed to load texture: ") + filePath);
        return;
    }
    this->textures.push_back(tex);
    Logger::log(std::string("[Engine] Loaded texture: ") + filePath);
}

void Engine::addMeshFromPrimitive(const Primitive &primitive, const std::string &name) {
    // Check if a mesh with the same name already exists
    if (!name.empty() && this->findMeshByName(name) != nullptr) {
        Logger::log(std::string("[Engine] Mesh already exists with name: ") + name);
        throw std::runtime_error("Mesh with the same name already exists");
    }

    Mesh* mesh = new Mesh(primitive, name);
    this->meshes.push_back(mesh);
    Logger::log(std::string("[Engine] Added mesh from primitive: ") + (name.empty() ? "Unnamed" : name));
}

void Engine::addMeshFromObjFile(const std::string& filePath, const std::string& name) {
    if (filePath.empty()) {
        Logger::log("[Engine] addMeshFromObjFile: empty path");
        return;
    }

    LoadedObjData loaded;
    try {
        loaded = loadObjWithMaterial(filePath.c_str());
    } catch (const std::exception& ex) {
        Logger::log(std::string("[Engine] Failed to load OBJ: ") + ex.what());
        throw;
    }

    if (loaded.vertices.empty()) {
        Logger::log(std::string("[Engine] OBJ has no vertices: ") + filePath);
        return;
    }

    std::string meshName = name;
    if (meshName.empty())
        meshName = basenameOfPath(filePath);
    if (meshName.empty())
        meshName = "OBJ";

    if (this->findMeshByName(meshName) != nullptr) {
        const std::string base = meshName;
        int suffix = 1;
        while (this->findMeshByName(meshName) != nullptr)
            meshName = base + "_" + std::to_string(suffix++);
    }

    Primitive custom;
    custom.set(loaded.vertices.data(), loaded.vertices.size(), nullptr, 0);
    this->addMeshFromPrimitive(custom, meshName);

    if (loaded.material.has_value()) {
        const ObjMaterialData& mat = loaded.material.value();
        if (!mat.name.empty() && this->findMaterialByName(mat.name) == nullptr) {
            this->materials.push_back(new Material(
                mat.Ka,
                mat.Kd,
                mat.Ks,
                mat.Ns,
                mat.d,
                mat.illum,
                0,
                1,
                mat.name
            ));
            Logger::log(std::string("[Engine] Created material from MTL: ") + mat.name);
        }

        Mesh* mesh = this->findMeshByName(meshName);
        if (mesh && !mat.name.empty()) {
            mesh->setMaterial(mat.name);
            Logger::log(std::string("[Engine] Linked material '") + mat.name + "' to mesh '" + meshName + "'");
        }
    }
}

void Engine::removeMeshWithName(std::string name) {
    auto it = std::find_if(this->meshes.begin(), this->meshes.end(), [&name](Mesh *mesh) {
        return mesh && mesh->getName() == name;
    });
    if (it == this->meshes.end()) {
        Logger::log(std::string("[Engine] removeMeshWithName: mesh not found: ") + name);
        return;
    }

    Mesh* meshToDelete = *it;
    this->selectedMeshs.erase(
        std::remove(this->selectedMeshs.begin(), this->selectedMeshs.end(), meshToDelete),
        this->selectedMeshs.end()
    );

    delete meshToDelete;
    this->meshes.erase(it);
    Logger::log(std::string("[Engine] Removed mesh: ") + name);
}

Mesh *Engine::findMeshByName(const std::string &name) {
    for (Mesh *mesh : this->meshes) {
        if (mesh->getName() ==  name)
            return mesh;
    }
    return nullptr;
}

Material *Engine::findMaterialByName(const std::string &name) {
    for (Material *material : this->materials) {
        if (material && material->getName() == name)
            return material;
    }
    return nullptr;
}

Texture *Engine::findTextureByName(const std::string &name) {
    for (Texture *texture : this->textures) {
        if (texture->getName() ==  name)
            return texture;
    }
    return nullptr;
}

bool Engine::removeTextureWithName(const std::string& name) {
    auto it = std::find_if(this->textures.begin(), this->textures.end(), [&name](Texture* texture) {
        return texture && texture->getName() == name;
    });
    if (it == this->textures.end()) {
        Logger::log(std::string("[Engine] removeTextureWithName: texture not found: ") + name);
        return false;
    }
    delete *it;
    this->textures.erase(it);
    Logger::log(std::string("[Engine] Removed texture: ") + name);
    return true;
}


std::vector<Texture *> Engine::getTextures() const {
    return this->textures;
}

std::vector<Material *> Engine::getMaterials() const {
    return this->materials;
}

std::vector<Mesh *> Engine::getMeshes() const {
    return this->meshes;
}

std::vector<Vec3> Engine::getLightsPositions() const {
    std::vector<Vec3> out;
    out.reserve(this->lightsPositions.size());
    for (Vec3* p : this->lightsPositions) {
        out.push_back(p ? *p : Vec3(0.f));
    }
    return out;
}

void Engine::addLight(const Vec3& position) {
    this->lightsPositions.push_back(new Vec3(position));
    if (!this->shaders.empty() && this->lightsPositions.size() == 1) {
        this->shaders[0]->setVec3f(*this->lightsPositions[0], "lightPos0");
    }
    Logger::log(std::string("[Engine] Added light at (")
        + std::to_string(position.x) + ", "
        + std::to_string(position.y) + ", "
        + std::to_string(position.z) + ")");
}

bool Engine::removeLightAtIndex(size_t index) {
    if (index >= this->lightsPositions.size()) {
        Logger::log("[Engine] removeLightAtIndex: index out of range");
        return false;
    }
    Vec3* victim = this->lightsPositions[index];
    delete victim;
    this->lightsPositions.erase(this->lightsPositions.begin() + index);

    if (!this->shaders.empty()) {
        if (!this->lightsPositions.empty()) {
            this->shaders[0]->setVec3f(*this->lightsPositions[0], "lightPos0");
        } else {
            this->shaders[0]->setVec3f(Vec3(0.f), "lightPos0");
        }
    }

    Logger::log(std::string("[Engine] Removed light #") + std::to_string(index));
    return true;
}

bool Engine::setLightPosition(size_t index, const Vec3& position) {
    if (index >= this->lightsPositions.size() || this->lightsPositions[index] == nullptr) {
        Logger::log("[Engine] setLightPosition: index out of range");
        return false;
    }
    *this->lightsPositions[index] = position;

    if (!this->shaders.empty() && index == 0) {
        this->shaders[0]->setVec3f(*this->lightsPositions[0], "lightPos0");
    }

    Logger::log(std::string("[Engine] Moved light #") + std::to_string(index));
    return true;
}

void Engine::addSelectedMesh(Mesh *mesh) {
    if (mesh == nullptr)
        return ;
    if (std::find(this->selectedMeshs.begin(), this->selectedMeshs.end(), mesh) != this->selectedMeshs.end())
        return ;
    selectedMeshs.push_back(mesh);
}

void Engine::removeSelectedMesh(Mesh *mesh) {
    if (mesh == nullptr)
        return ;
    auto it = std::find(this->selectedMeshs.begin(), this->selectedMeshs.end(), mesh);
    if (it != this->selectedMeshs.end())
        this->selectedMeshs.erase(it);
}

void Engine::clearSelectedMeshs() {
    this->selectedMeshs.clear();
}
