#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "maths.hpp"
#include "Shader.hpp"

class Material {
    private:
        Vec3 Ka; // Ambient reflectivity
        Vec3 Kd; // Diffuse reflectivity
        Vec3 Ks; // Specular reflectivity
        float Ns; // Shininess
        float d;  // Opacity
        int illum; // Illumination model

        GLint diffuseTex;
        GLint specularTex;
    public:
        Material(Vec3 Ka,
            Vec3 Kd,
            Vec3 Ks,
            float Ns,
            float d,
            int illum,

            GLint diffuseTex,
            GLint specularTex);
        ~Material();
        
        void sendToShader(Shader &program);
};
