#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <vector>
#include "maths.hpp"

class Primitive {
    private:
        std::vector<Vertex> vertices;
        std::vector<GLuint> indices;

    public:
        Primitive();
        virtual ~Primitive();

        void set(const Vertex *vertices, const unsigned int nrOfVertices, const GLuint *indices, const unsigned nrOfIndices);
        const Vertex *getVertices() const;
        const GLuint *getIndices() const;
        unsigned getNrOfVertices() const;
        unsigned getNrOfIndices() const;
};

class Quad : public Primitive {
    public:
        Quad();
};

class Triangle : public Primitive {
    public:
        Triangle();
};

class Pyramid : public Primitive {
    public:
        Pyramid();
};

class Cube : public Primitive {
    public:
        Cube();
};
