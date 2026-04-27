#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <vector>
#include <memory>

#include "maths.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "Material.hpp"
#include "Primitives.hpp"
#include "Texture.hpp"

struct MeshRenderState {
    GLenum drawMode = GL_FILL;
    int renderMode = 0;
    bool debugLight = true;
    bool culling = true;
    std::string materialName = "";
    std::string textureName = "";
};

class Mesh {
    private:
        std::string name;
        unsigned nrOfVertices;
        unsigned nrOfIndices;

        GLuint VAO;
        GLuint VBO;
        GLuint EBO;

        Vec3 position;
        Vec3 rotation;
        Vec3 scale;
        Mat4 ModelMatrix;

        MeshRenderState renderState;

        void initVAO(Vertex *vertexArray, const unsigned &nrOfVertices, 
            GLuint* indexArray, const unsigned &nrOfIndices);
        void initVAO(Primitive *primitive);
        void initVAO(const Primitive &primitive);
        
        void updateUniforms(Shader *shader);
        void updateModelMatrix();

    public:
        Mesh(Vertex *vertexArray,
            const unsigned &nrOfVertices, 
            GLuint* indexArray, 
            const unsigned &nrOfIndices,
            std::string name = "Undifined",
            Vec3 position = Vec3(0.f), 
            Vec3 rotation = Vec3(0.f), 
            Vec3 scale = Vec3(1.f));

        Mesh(Primitive *primitive,
            std::string name = "Undifined",
            Vec3 position = Vec3(0.f), 
            Vec3 rotation = Vec3(0.f), 
            Vec3 scale = Vec3(1.f));

        Mesh(const Primitive &primitive,
            std::string name = "Undifined",
            Vec3 position = Vec3(0.f), 
            Vec3 rotation = Vec3(0.f), 
            Vec3 scale = Vec3(1.f));
        ~Mesh();

        std::string getName() const;

        const Vec3 &getPosition() const;
        const Vec3 &getRotation() const;
        const Vec3 &getScale() const;
        const std::string &getMaterialName() const;
        const std::string getTextureName() const;

        GLenum getDrawMode() const;
        int getRenderMode() const;
        bool getDebugLight() const;
        bool getCullingMode() const;
        
        void setPosition(const Vec3 position);
        void setRotation(const Vec3 rotation);
        void setScale(const Vec3 scale);
        void setMaterial(const std::string &materialName);
        void setTexture(std::string textureName);
        
        void setDrawMode(const GLenum drawMode);
        void setRenderMode(const int renderMode);
        void setDebugLight(bool enabled);
        void setCullingMode(bool enabled);

        void move(const Vec3 translation);
        void rotate(const Vec3 rotation);
        void scaleUp(const Vec3 scale);

        void render(Shader *shader, GLenum drawMode = GL_FILL);
        void render(Shader *shader, const Mat4 &modelMatrix, GLenum drawMode = GL_FILL);
};