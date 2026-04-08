#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <memory>

#include "maths.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "Material.hpp"
#include "Primitives.hpp"

class Mesh {
    private:
        unsigned nrOfVertices;
        unsigned nrOfIndices;

        GLuint VAO;
        GLuint VBO;
        GLuint EBO;

        Vec3 position;
        Vec3 rotation;
        Vec3 scale;
        Mat4 ModelMatrix;

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
            Vec3 position = Vec3(0.f), 
            Vec3 rotation = Vec3(0.f), 
            Vec3 scale = Vec3(1.f));

        Mesh(Primitive *primitive,
            Vec3 position = Vec3(0.f), 
            Vec3 rotation = Vec3(0.f), 
            Vec3 scale = Vec3(1.f));

        Mesh(const Primitive &primitive,
            Vec3 position = Vec3(0.f), 
            Vec3 rotation = Vec3(0.f), 
            Vec3 scale = Vec3(1.f));
        ~Mesh();

        const Vec3 &getPosition() const;
        const Vec3 &getRotation() const;
        const Vec3 &getScale() const;
        
        void setPosition(const Vec3 position);
        void setRotation(const Vec3 rotation);
        void setScale(const Vec3 scale);

        void move(const Vec3 translation);
        void rotate(const Vec3 rotation);
        void scaleUp(const Vec3 scale);

        void update();
        void render(Shader *shader, GLenum drawMode = GL_FILL);
        void render(Shader *shader, const Mat4 &modelMatrix, GLenum drawMode = GL_FILL);
};