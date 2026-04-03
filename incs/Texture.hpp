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
        GLint textureUnit;

    public:
        Texture(const char *filePath, GLenum type, GLint texture_unit);
        ~Texture();
        void loadFromFile(const char *filePath);
        GLuint getID() const;
        void bind();
        void unbind();
        GLint getTextureUnit() const;

};