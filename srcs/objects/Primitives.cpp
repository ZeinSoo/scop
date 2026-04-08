#include "Primitives.hpp"

///////////////////////////////////////////
///////                             ///////
///////           PRIMITIVE         ///////
///////                             ///////
///////////////////////////////////////////

Primitive::Primitive() {

}

Primitive::~Primitive() {

}

void Primitive::set(const Vertex *vertices, const unsigned int nrOfVertices, const GLuint *indices, const unsigned nrOfIndices) {
    for (size_t i = 0; i < nrOfVertices; i++) {
        this->vertices.push_back(vertices[i]);
    }

    for (size_t i = 0; i < nrOfIndices; i++) {
        this->indices.push_back(indices[i]);
    }
}

const Vertex *Primitive::getVertices() const{
    return this->vertices.data();
}

const GLuint *Primitive::getIndices() const{
    return this->indices.data();
}

unsigned Primitive::getNrOfVertices() const{
    return this->vertices.size();
}

unsigned Primitive::getNrOfIndices() const{
    return this->indices.size();
}


///////////////////////////////////////////
///////                             ///////
///////           QUAD              ///////
///////                             ///////
///////////////////////////////////////////

Quad::Quad() : Primitive() {
    Vertex vertices[] = {
        Vec3(-0.5f, 0.5f, 0.0f), Vec3(1.0f, 1.0f, 1.0f), Vec2(0.0f, 1.0f), Vec3(0.f, 0.f, 1.f),
        Vec3(-0.5f, -0.5f, 0.0f), Vec3(0.0f, 0.0f, 1.0f), Vec2(0.0f, 0.0f), Vec3(0.f, 0.f, 1.f),
        Vec3(0.5f, -0.5f, 0.0f), Vec3(1.0f, 0.0f, 0.0f), Vec2(1.0f, 0.0f), Vec3(0.f, 0.f, 1.f),
        Vec3(0.5f, 0.5f, 0.0f), Vec3(1.0f, 1.0f, 1.0f), Vec2(1.0f, 1.0f), Vec3(0.f, 0.f, 1.f)
    };
    unsigned nrOfVertices = sizeof(vertices) / sizeof(Vertex);

    GLuint indices[] = {
        0, 1, 2,
        0, 2, 3,
    };
    unsigned nrOfIndices = sizeof(indices) / sizeof(GLuint);

    this->set(vertices, nrOfVertices, indices, nrOfIndices);
}


///////////////////////////////////////////
///////                             ///////
///////           PYRAMID           ///////
///////                             ///////
///////////////////////////////////////////

Pyramid::Pyramid() : Primitive() {
    Vertex vertices[] = {
        //Position                  //Color                  //TexCoords              //Normal
        Vec3(-0.5f, 0.f, 0.5f),     Vec3(1.f, 0.f, 0.f),    Vec2(1.0f, 0.0f),       Vec3(0.f, 0.f, 1.f),                                          
        Vec3(-0.5f, 0.f, -0.5f),    Vec3(0.f, 1.f, 0.f),    Vec2(1.0f, 0.0f),       Vec3(0.f, 0.f, 1.f),
        Vec3(0.5f, 0.f, -0.5f),     Vec3(0.f, 0.f, 1.f),    Vec2(0.0f, 1.0f),       Vec3(0.f, 0.f, 1.f),
        Vec3(0.5f, 0.f, 0.5f),      Vec3(1.f, 0.f, 1.f),    Vec2(0.0f, 1.0f),       Vec3(0.f, 0.f, 1.f),
        Vec3(0.f, 0.5f, 0.f),       Vec3(1.f, 1.f, 1.f),    Vec2(0.5f, 0.5f),       Vec3(0.f, 0.f, 1.f)
    };
    unsigned nrOfVertices = sizeof(vertices) / sizeof(Vertex);

    GLuint indices[] = {
        4, 1, 0,
        4, 2, 1,
        4, 3, 2,
        4, 0, 3,
        0, 1, 2,
        0, 2, 3
    };
    unsigned nrOfIndices = sizeof(indices) / sizeof(GLuint);

    this->set(vertices, nrOfVertices, indices, nrOfIndices);
}

///////////////////////////////////////////
///////                             ///////
///////           CUBE              ///////
///////                             ///////
///////////////////////////////////////////

Cube::Cube() : Primitive() {
    Vertex vertices[] = {
        Vec3(-0.5f, -0.5f, 0.5f), Vec3(1.0f, 0.0f, 0.0f), Vec2(0.0f, 0.0f),Vec3(0.f, 0.f, 1.f), // 0
        Vec3(0.5f, -0.5f, 0.5f), Vec3(0.0f, 1.0f, 0.0f), Vec2(1.0f, 0.0f),Vec3(0.f, 0.f, 1.f),  // 1
        Vec3(0.5f, 0.5f, 0.5f), Vec3(0.0f, 0.0f, 1.0f), Vec2(1.0f, 1.0f), Vec3(0.f, 0.f, 1.f),  // 2
        Vec3(-0.5f, 0.5f, 0.5f), Vec3(1.0f, 1.0f, 0.0f), Vec2(0.0f, 1.0f),Vec3(0.f, 0.f, 1.f),  // 3
        Vec3(-0.5f, -0.5f, -0.5f), Vec3(1.0f, 0.0f, 1.0f), Vec2(0.0f, 0.0f), Vec3(0.f, 0.f, 1.f),// 4
        Vec3(0.5f, -0.5f, -0.5f), Vec3(0.0f, 1.0f, 1.0f), Vec2(1.0f, 0.0f), Vec3(0.f, 0.f, 1.f), // 5
        Vec3(0.5f, 0.5f, -0.5f), Vec3(1.0f, 1.0f, 1.0f), Vec2(1.0f, 1.0f), Vec3(0.f, 0.f, 1.f),  // 6
        Vec3(-0.5f, 0.5f, -0.5f), Vec3(0.2f, 0.2f, 0.2f), Vec2(0.0f, 1.0f), Vec3(0.f, 0.f, 1.f)   // 7
    };
    unsigned nrOfVertices = sizeof(vertices) / sizeof(Vertex);

    GLuint indices[] = {
        // Front (+Z)
        0, 1, 2,
        0, 2, 3,
        4, 6, 5,
        4, 7, 6,
        4, 0, 3,
        4, 3, 7,
        1, 5, 6,
        1, 6, 2,
        3, 2, 6,
        3, 6, 7,
        4, 5, 1,
        4, 1, 0
    };
    unsigned nrOfIndices = sizeof(indices) / sizeof(GLuint);

    this->set(vertices, nrOfVertices, indices, nrOfIndices);
}