#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>

#include "maths.hpp"
#include "BMPLoader.hpp"

class Texture {
    private:
        GLuint id;
        int width;
        int height;
        unsigned int type;

    public:
        Texture(const char *filePath, GLenum type);
        ~Texture();
        void loadFromFile(const char *filePath);
        GLuint getID() const;
        void bind(GLint texture_unit);
        void unbind();

};