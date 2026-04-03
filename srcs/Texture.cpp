#include "Texture.hpp"

Texture::Texture(const char *filePath, GLenum type, GLint texture_unit) {
    this->type = type;
    this->textureUnit = texture_unit;
    unsigned char *imageData = loadTexture(filePath, &this->width, &this->height);
    if (!imageData) {
        std::cout << "ERROR::TEXTURE::TEXTURE_LOADING_FAILED: " << filePath <<  std::endl;
    }

    glGenTextures(1, &this->id);
    glBindTexture(GL_TEXTURE_2D, this->id);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexParameteri(this->type, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(this->type, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(this->type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(this->type, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glTexImage2D(this->type, 0, GL_RGBA, this->width, this->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
    glGenerateMipmap(this->type);

    glBindTextureUnit(0, 0);
    delete[] imageData;
    imageData = NULL;
}

Texture::~Texture() {
    glDeleteTextures(1, &this->id);
}

void Texture::loadFromFile(const char *filePath) {
    if (this->id) {
        glDeleteTextures(1, &this->id);
        this->id = 0;
    }

    unsigned char *imageData = loadTexture(filePath, &this->width, &this->height);
    if (!imageData) {
        std::cout << "ERROR::TEXTURE::LOADFROMFILE::TEXTURE_LOADING_FAILED: " << filePath <<  std::endl;
    }

    glGenTextures(1, &this->id);
    glBindTexture(GL_TEXTURE_2D, this->id);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexParameteri(this->type, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(this->type, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(this->type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(this->type, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glTexImage2D(this->type, 0, GL_RGBA, this->width, this->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
    glGenerateMipmap(this->type);

    glBindTextureUnit(0, 0);
    delete[] imageData;
    imageData = NULL;
}

GLuint Texture::getID() const {
    return this->id;
}

void Texture::bind() {
    // glActiveTexture(GL_TEXTURE0 + texture_unit);
    // glBindTexture(type, this->id);

    glBindTextureUnit(this->textureUnit, this->id);
}

void Texture::unbind() {
    glBindTextureUnit(0, 0);
}

GLint Texture::getTextureUnit() const {
    return this->textureUnit;
}