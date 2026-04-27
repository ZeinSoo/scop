#include "Mesh.hpp"

Mesh::Mesh(Vertex *vertexArray, const unsigned &nrOfVertices,
            GLuint* indexArray, const unsigned &nrOfIndices,
            std::string name,
            Vec3 position,
            Vec3 rotation,
            Vec3 scale)
{
    this->name = name;
    this->position = position;
    this->rotation = rotation;
    this->scale = scale;

    this->initVAO(vertexArray, nrOfVertices, indexArray, nrOfIndices);
    this->updateModelMatrix();
}

Mesh::Mesh(Primitive *primitive,
            std::string name,
            Vec3 position,
            Vec3 rotation,
            Vec3 scale)
{
    this->name = name;
    this->position = position;
    this->rotation = rotation;
    this->scale = scale;

    this->initVAO(primitive);
    this->updateModelMatrix();
}

Mesh::Mesh(const Primitive &primitive,
            std::string name,
            Vec3 position,
            Vec3 rotation,
            Vec3 scale)
{
    this->name = name;
    this->position = position;
    this->rotation = rotation;
    this->scale = scale;
    
    this->initVAO(primitive);
    this->updateModelMatrix();
}


Mesh::~Mesh() {
    glDeleteVertexArrays(1, &this->VAO);
    glDeleteBuffers(1, &this->VBO);
    glDeleteBuffers(1, &this->EBO);
}

std::string Mesh::getName() const {
    return this->name;
}

const Vec3 &Mesh::getPosition() const {
    return this->position;
}

const Vec3 &Mesh::getRotation() const {
    return this->rotation;
}

const Vec3 &Mesh::getScale() const {
    return this->scale;
}

const std::string &Mesh::getMaterialName() const {
    return this->renderState.materialName;
}

const std::string Mesh::getTextureName() const {
    return this->renderState.textureName;
}

GLenum Mesh::getDrawMode() const {
    return this->renderState.drawMode;
}

bool Mesh::getDebugLight() const{
    return this->renderState.debugLight;
}

bool Mesh::getCullingMode() const {
    return this->renderState.culling;
}

void Mesh::initVAO(Vertex *vertexArray, const unsigned &nrOfVertices, 
            GLuint* indexArray, const unsigned &nrOfIndices) {
    
    // Set variables
    this->nrOfVertices = nrOfVertices;
    this->nrOfIndices = nrOfIndices;
            
    // VAO (Vertex Array Object), VBO (Vertex Buffer Array), EBO (Element Buffer Object)
    // GEN VAO AND BIND
    glCreateVertexArrays(1, &this->VAO);
    glBindVertexArray(this->VAO);

    // GEN VBO AND BIND AND SEND DATA
    glGenBuffers(1, &this->VBO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, this->nrOfVertices * sizeof(Vertex), vertexArray, GL_STATIC_DRAW);
    // glBufferData(GL_ARRAY_BUFFER, temp.size() * sizeof(Vertex), temp.data(), GL_STATIC_DRAW);

    // GEN EBO AND BIND AND SEND DATA
    glGenBuffers(1, &this->EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->nrOfIndices * sizeof(GLuint), indexArray, GL_STATIC_DRAW);

    // SET VERTEX ATTRIBUTES POINTERS AND ENABLE, (INPUT ASSEMBLY)
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);

    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, color));
    glEnableVertexAttribArray(1);

    // Texture coordinates attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, texCoords));
    glEnableVertexAttribArray(2);

    // Normal
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(3);

    // BIND VAO 0
    glBindVertexArray(0);
}

void Mesh::initVAO(Primitive *primitive) {
    
    // Set variables
    this->nrOfVertices = primitive->getNrOfVertices();
    this->nrOfIndices = primitive->getNrOfIndices();
            
    // VAO (Vertex Array Object), VBO (Vertex Buffer Array), EBO (Element Buffer Object)
    // GEN VAO AND BIND
    glCreateVertexArrays(1, &this->VAO);
    glBindVertexArray(this->VAO);

    // GEN VBO AND BIND AND SEND DATA
    glGenBuffers(1, &this->VBO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, primitive->getNrOfVertices() * sizeof(Vertex), primitive->getVertices(), GL_STATIC_DRAW);
    // glBufferData(GL_ARRAY_BUFFER, temp.size() * sizeof(Vertex), temp.data(), GL_STATIC_DRAW);

    // GEN EBO AND BIND AND SEND DATA
    glGenBuffers(1, &this->EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, primitive->getNrOfIndices() * sizeof(GLuint), primitive->getIndices(), GL_STATIC_DRAW);

    // SET VERTEX ATTRIBUTES POINTERS AND ENABLE, (INPUT ASSEMBLY)
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);

    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, color));
    glEnableVertexAttribArray(1);

    // Texture coordinates attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, texCoords));
    glEnableVertexAttribArray(2);

    // Normal
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(3);

    // BIND VAO 0
    glBindVertexArray(0);
}

void Mesh::initVAO(const Primitive &primitive) {
    
    // Set variables
    this->nrOfVertices = primitive.getNrOfVertices();
    this->nrOfIndices = primitive.getNrOfIndices();
            
    // VAO (Vertex Array Object), VBO (Vertex Buffer Array), EBO (Element Buffer Object)
    // GEN VAO AND BIND
    glCreateVertexArrays(1, &this->VAO);
    glBindVertexArray(this->VAO);

    // GEN VBO AND BIND AND SEND DATA
    glGenBuffers(1, &this->VBO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, primitive.getNrOfVertices() * sizeof(Vertex), primitive.getVertices(), GL_STATIC_DRAW);
    // glBufferData(GL_ARRAY_BUFFER, temp.size() * sizeof(Vertex), temp.data(), GL_STATIC_DRAW);

    // GEN EBO AND BIND AND SEND DATA
    glGenBuffers(1, &this->EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, primitive.getNrOfIndices() * sizeof(GLuint), primitive.getIndices(), GL_STATIC_DRAW);

    // SET VERTEX ATTRIBUTES POINTERS AND ENABLE, (INPUT ASSEMBLY)
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);

    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, color));
    glEnableVertexAttribArray(1);

    // Texture coordinates attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, texCoords));
    glEnableVertexAttribArray(2);

    // Normal
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(3);

    // BIND VAO 0
    glBindVertexArray(0);
}

void Mesh::updateUniforms(Shader *shader) {
    shader->setMat4f(this->ModelMatrix, "ModelMatrix");
}

void Mesh::updateModelMatrix() {
    this->ModelMatrix = Mat4(1.f);
    this->ModelMatrix = ModelMatrix.translate(ModelMatrix, this->position);
    this->ModelMatrix = ModelMatrix.rotate(ModelMatrix, radians(this->rotation.x), Vec3(1.f, 0.f, 0.f));
    this->ModelMatrix = ModelMatrix.rotate(ModelMatrix, radians(this->rotation.y), Vec3(0.f, 1.f, 0.f));
    this->ModelMatrix = ModelMatrix.rotate(ModelMatrix, radians(this->rotation.z), Vec3(0.f, 0.f, 1.f));
    this->ModelMatrix = ModelMatrix.scale(ModelMatrix, this->scale);
}

void Mesh::setPosition(const Vec3 position) {
    this->position = position;
}

void Mesh::setRotation(const Vec3 rotation) {
    this->rotation = rotation;
}

void Mesh::setScale(const Vec3 scale) {
    this->scale = scale;
}

void Mesh::setMaterial(const std::string &materialName) {
    this->renderState.materialName = materialName;
}

void Mesh::setTexture(std::string textureName) {
    this->renderState.textureName = textureName;
}

void Mesh::setDrawMode(const GLenum drawMode) {
    this->renderState.drawMode = drawMode;
}

int Mesh::getRenderMode() const {
    return this->renderState.renderMode;
}

void Mesh::setRenderMode(const int renderMode) {
    this->renderState.renderMode = renderMode;
}

void Mesh::setDebugLight(bool enabled) {
    this->renderState.debugLight = enabled;
}

void Mesh::setCullingMode(bool enabled) {
    this->renderState.culling = enabled;
}

void Mesh::move(const Vec3 translation) {
    this->position = this->position + translation;
}

void Mesh::rotate(const Vec3 rotation) {
    this->rotation = this->rotation + rotation;
    if (this->rotation.x >= 360.f)
        this->rotation.x -= 360.f;
    if (this->rotation.y >= 360.f)
        this->rotation.y -= 360.f;
    if (this->rotation.z >= 360.f)
        this->rotation.z -= 360.f;
}

void Mesh::scaleUp(const Vec3 scale) {
    this->scale = this->scale + scale;
}

void Mesh::render(Shader *shader, GLenum drawMode) {
    this->updateModelMatrix();
    this->updateUniforms(shader);

    shader->use();

    glBindVertexArray(this->VAO);

    // DRAW
    glPolygonMode(GL_FRONT_AND_BACK, drawMode);
    if (nrOfIndices == 0) {
        glDrawArrays(GL_TRIANGLES, 0, this->nrOfVertices);
    }
    else {
        glDrawElements(GL_TRIANGLES, this->nrOfIndices , GL_UNSIGNED_INT, 0);
    }
}

void Mesh::render(Shader *shader, const Mat4 &modelMatrix, GLenum drawMode) {
    shader->setMat4f(modelMatrix, "ModelMatrix");
    shader->use();

    glBindVertexArray(this->VAO);
    glPolygonMode(GL_FRONT_AND_BACK, drawMode);
    if (nrOfIndices == 0) {
        glDrawArrays(GL_TRIANGLES, 0, this->nrOfVertices);
    }
    else {
        glDrawElements(GL_TRIANGLES, this->nrOfIndices , GL_UNSIGNED_INT, 0);
    }
}