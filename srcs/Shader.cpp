#include "../incs/Shader.hpp"

Shader::Shader(const char *vertexFile, const char *fragmentFile, const char *geometryFile) {
    GLuint vertexShader, fragmentShader, geometryShader = 0;

    vertexShader = loadShader(GL_VERTEX_SHADER, vertexFile);
    fragmentShader = loadShader(GL_FRAGMENT_SHADER, fragmentFile);

    if (std::string(geometryFile) != "")
        geometryShader = loadShader(GL_GEOMETRY_SHADER, geometryFile);

    linkProgram(vertexShader, fragmentShader, geometryShader);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glDeleteShader(geometryShader);
}

Shader::~Shader() {
    glDeleteProgram(this->id);
}

std::string Shader::loadShaderSource(const char *filePath) {
    std::string temp = "";
    std::string src = "";
    std::ifstream in_file;

    in_file.open(filePath);
    if (in_file.is_open()) {
        while (std::getline(in_file, temp))
            src += temp + "\n";
    } else {
        std::cout << "ERROR::SHADERS::COULD_NOT_OPEN_FILE: " << filePath << std::endl;
    }
    in_file.close();

    return src;
}

GLuint Shader::loadShader(GLenum type, const char *filePath) {
    char infoLog[512];
    GLint success;

    GLuint shader = glCreateShader(type);
    std::string str_src = this->loadShaderSource(filePath);
    const GLchar *src = str_src.c_str();

    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADERS::COULD_NOT_COMPILE_SHADER: " << filePath << '\n' << infoLog << std::endl;
    }

    return shader;
}

void Shader::linkProgram(GLuint vertexShader, GLuint fragmentShader, GLuint geometryShader) {
    char infoLog[512];
    GLint success;

    this->id = glCreateProgram();

    if (vertexShader)
        glAttachShader(this->id, vertexShader);
    if (fragmentShader)
        glAttachShader(this->id, fragmentShader);
    if (geometryShader)
        glAttachShader(this->id, geometryShader);

    glLinkProgram(this->id);
    
    glGetProgramiv(this->id, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(this->id, 512, NULL, infoLog);
        std::cout << "ERROR::SHADERS::COULD_NOT_LINK_PROGRAM\n" << infoLog << std::endl;
    }

    unuse();
}

void Shader::use() const {
    glUseProgram(this->id);
}

void Shader::unuse() const {
    glUseProgram(0);
}

GLuint Shader::getId() const {
    return this->id;
}

void Shader::setVec3f(Vec3 vec, const GLchar *name) const {
    float vecArr[3] = {vec.x, vec.y, vec.z};
    const GLint location = glGetUniformLocation(this->id, name);
    if (location != -1)
        glProgramUniform3fv(this->id, location, 1, vecArr);
}

void Shader::setMat4f(Mat4 mat, const GLchar *name, bool transpose) const {
    const GLint location = glGetUniformLocation(this->id, name);
    if (location != -1)
        glProgramUniformMatrix4fv(this->id, location, 1, transpose, mat.m);
}
    
void Shader::set1i(GLint value, const GLchar *name) const {
    const GLint location = glGetUniformLocation(this->id, name);
    if (location != -1)
        glProgramUniform1i(this->id, location, value);
}

void Shader::set1f(GLfloat value, const GLchar *name) const {
    const GLint location = glGetUniformLocation(this->id, name);
    if (location != -1)
        glProgramUniform1f(this->id, location, value);
}