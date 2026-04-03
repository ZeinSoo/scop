#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>

#include "maths.hpp"

class Shader {
    private:
        GLuint id;

        std::string loadShaderSource(const char *filePath);

        GLuint loadShader(GLenum type, const char *filePath);

        void linkProgram(GLuint vertexShader, GLuint fragmentShader, GLuint geometryShader);

    public:
        Shader(const char *vertexFile, const char *fragmentFile, const char *geometryFile);
        ~Shader();

        void use() const;
        void unuse() const;

        GLuint getId() const;

        void setVec3f(Vec3 vec, const GLchar *name) const;
        void setMat4f(Mat4 mat, const GLchar *name, bool transpose = GL_FALSE) const;
        void set1i(GLint value, const GLchar *name) const;
};