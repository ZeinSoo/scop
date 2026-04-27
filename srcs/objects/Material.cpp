#include "Material.hpp"

Material::Material(Vec3 Ka,
            Vec3 Kd,
            Vec3 Ks,
            float Ns,
            float d,
            int illum,

            GLint diffuseTex,
            GLint specularTex,
            std::string name
            )
        {
            this->Ka = Ka;
            this->Kd = Kd;
            this->Ks = Ks;
            this->Ns = Ns;
            this->d = d;
            this->illum = illum;
            this->diffuseTex = diffuseTex;
            this->specularTex = specularTex;
            this->name = name;
        }

Material::~Material() {

}

const std::string &Material::getName() const {
    return this->name;
}

GLint Material::getDiffuseTexUnit() const {
    return this->diffuseTex;
}

GLint Material::getSpecularTexUnit() const {
    return this->specularTex;
}

void Material::sendToShader(Shader &program) {
    program.setVec3f(this->Ka, "material.Ka");
    program.setVec3f(this->Kd, "material.Kd");
    program.setVec3f(this->Ks, "material.Ks");
    program.set1f(this->Ns, "material.Ns");
    program.set1f(this->d, "material.d");
    program.set1i(this->illum, "material.illum");
    program.set1i(this->diffuseTex, "material.diffuseTex");
    program.set1i(this->specularTex, "material.specularTex");
}