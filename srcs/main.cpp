//valgrind --leak-check=full --show-leak-kinds=definite,indirect ./scop

#include "libs.hpp"

// Vertex vertices[] = {
//     Vec3(-0.5f, 0.5f, 0.0f), Vec3(1.0f, 1.0f, 1.0f), Vec2(0.0f, 1.0f),  
//     Vec3(-0.5f, -0.5f, 0.0f), Vec3(0.0f, 0.0f, 1.0f), Vec2(0.0f, 0.0f),
//     Vec3(0.5f, -0.5f, 0.0f), Vec3(1.0f, 0.0f, 0.0f), Vec2(1.0f, 0.0f),
//     Vec3(0.5f, 0.5f, 0.0f), Vec3(1.0f, 1.0f, 1.0f), Vec2(1.0f, 1.0f)
// };
Vertex vertices[] = {
    Vec3(-0.5f, 0.f, 0.5f), Vec3(1.0f, 0.0f, 0.0f), Vec2(0.0f, 1.0f),
    Vec3(-0.5f, 0.f, -0.5f), Vec3(0.0f, 1.0f, 0.0f), Vec2(0.0f, 1.0f),
    Vec3(0.5f, 0.f, -0.5f), Vec3(0.0f, 0.0f, 1.0f), Vec2(0.0f, 1.0f),
    Vec3(0.5f, 0.f, 0.5f), Vec3(1.0f, 0.0f, 1.0f), Vec2(0.0f, 1.0f),
    Vec3(0.f, 0.5f, 0.f), Vec3(1.0f, 1.0f, 1.0f), Vec2(0.0f, 1.0f)
};

unsigned nrOfVertices = sizeof(vertices) / sizeof(Vertex);

GLuint indices[] = {
    0, 1, 2,
    0, 2, 3,
    0, 1, 4,
    1, 2, 4,
    2, 3, 4,
    3, 0, 4
};
unsigned nrOfIndices = sizeof(indices) / sizeof(GLuint);

GLenum DRAW_MODE;

void updateInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        DRAW_MODE = GL_FILL;
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        DRAW_MODE = GL_LINE;
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
        DRAW_MODE = GL_POINT;
}

bool loadShaders(GLuint &core_program) {
    bool loadSuccess = true;
    char infoLog[512];
    GLint success;

    std::string temp = "";
    std::string src = "";
    std::ifstream in_file;

    // VERTEX SHADER
    in_file.open("resources/vertex_core.glsl");
    if (in_file.is_open()) {
        while (std::getline(in_file, temp))
            src += temp + "\n";
    } else {
        std::cout << "ERROR::MAIN.CPP::LOADSHADERS::COULD_NOT_OPEN_VERTEX_FILE" << std::endl;
        loadSuccess = false;
    }
    in_file.close();

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const GLchar *vertSrc = src.c_str();
    glShaderSource(vertexShader, 1, &vertSrc, NULL);
    glCompileShader(vertexShader);

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::MAIN.CPP::LOADSHADERS::VERTEX_SHADER_COMPILATION_FAILED\n" << infoLog << std::endl;
        loadSuccess = false;
    }

    temp = "";
    src = "";

    // FRAGMENT SHADER
    in_file.open("resources/fragment_core.glsl");
    if (in_file.is_open()) {
        while (std::getline(in_file, temp))
            src += temp + "\n";
    } else {
        std::cout << "ERROR::MAIN.CPP::LOADSHADERS::COULD_NOT_OPEN_FRAGMENT_FILE" << std::endl;
        loadSuccess = false;
    }
    in_file.close();

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const GLchar *fragSrc = src.c_str();
    glShaderSource(fragmentShader, 1, &fragSrc, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::MAIN.CPP::LOADSHADERS::FRAGMENT_SHADER_COMPILATION_FAILED\n" << infoLog << std::endl;
        loadSuccess = false;
    }

    // SHADER PROGRAM
    core_program = glCreateProgram();
    glAttachShader(core_program, vertexShader);
    glAttachShader(core_program, fragmentShader);
    glLinkProgram(core_program);

    glGetProgramiv(core_program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(core_program, 512, NULL, infoLog);
        std::cout << "ERROR::MAIN.CPP::LOADSHADERS::SHADER_PROGRAM_LINKING_FAILED\n" << infoLog << std::endl;
        return false;
    }

    glUseProgram(0);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return loadSuccess;
}

// #include <glm/glm.hpp>
// #include <glm/gtc/matrix_transform.hpp>
// #include <glm/gtc/type_ptr.hpp>
// #include <glm/gtx/string_cast.hpp>

// std::ostream &operator<<(std::ostream &os, const glm::mat4 &mat) {
//     os << "Mat4:" << std::endl;
//     for (int row = 0; row < 4; row++) {
//         for (int col = 0; col < 4; col++) {
//             os << mat[col][row] << " "; // GLM est column-major
//         }
//         os << std::endl;
//     }
//     return os;
// }

int main() {
    
    glfwInit();
    if (!glfwInit()) {
        std::cout << "ERROR::MAIN.CPP::GLFW_INIT_FAILED" << std::endl;
        return -1;
    }

    const int WIN_WIDTH = 1600;
    const int WIN_HEIGHT = 900;
    int framebufferWidth, framebufferHeight = 0;

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // A VERIFIER SUR LE PC
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow *window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "Scop", NULL, NULL);
    if (!window) {
        std::cout << "ERROR::MAIN.CPP::GLFW_WINDOW_CREATION_FAILED" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
    glViewport(0, 0, framebufferWidth, framebufferHeight);

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK) {
        std::cout << "ERROR::MAIN.CPP::GLEW_INIT_FAILED" << std::endl;
        glfwTerminate();
        return -1;
    }

    // OPENGL OPTIONS
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    DRAW_MODE = GL_FILL;
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // SHADERS INIT
    GLuint core_program;
    if (!loadShaders(core_program)) {
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }
    
    // MODEL

    // VAO (Vertex Array Object), VBO (Vertex Buffer Array), EBO (Element Buffer Object)
    // GEN VAO AND BIND
    GLuint VAO;
    glCreateVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // GEN VBO AND BIND AND SEND DATA
    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // GEN EBO AND BIND AND SEND DATA
    GLuint EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // SET VERTEX ATTRIBUTES POINTERS AND ENABLE (INPUT ASSEMBLY)
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);

    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, color));
    glEnableVertexAttribArray(1);

    // Texture coordinates attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, texCoords));
    glEnableVertexAttribArray(2);

    // BIND VAO 0
    glBindVertexArray(0);

    // TEXTURE INIT (TO DO)

    // Model Matrix

    Mat4 ModelMatrix(1.f);
    ModelMatrix = ModelMatrix.translate(ModelMatrix, Vec3(0.f, 0.f, 0.f));
    ModelMatrix = ModelMatrix.rotate(ModelMatrix, radians(0.f), Vec3(1.f, 0.f, 0.f));
    ModelMatrix = ModelMatrix.rotate(ModelMatrix, radians(0.f), Vec3(0.f, 1.f, 0.f));
    ModelMatrix = ModelMatrix.rotate(ModelMatrix, radians(0.f), Vec3(0.f, 0.f, 1.f));
    ModelMatrix = ModelMatrix.scale(ModelMatrix, Vec3(1.f));
    glUseProgram(core_program);

    glUniformMatrix4fv(glGetUniformLocation(core_program, "ModelMatrix"), 1, GL_FALSE, ModelMatrix.m);

    glUseProgram(0);

    // RENDER LOOP
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // UPDATE INPUT
        updateInput(window);

        //CLEAR
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        // USE PROGRAM
        glUseProgram(core_program);
        
        // UPDATE UNIFORMS (TO DO)

        // Move, Rotate, Scale
        ModelMatrix = ModelMatrix.translate(ModelMatrix, Vec3(0.f, 0.f, 0.f));
        ModelMatrix = ModelMatrix.rotate(ModelMatrix, radians(0.0f), Vec3(1.f, 0.f, 0.f));
        ModelMatrix = ModelMatrix.rotate(ModelMatrix, radians(0.1f), Vec3(0.f, 1.f, 0.f));
        ModelMatrix = ModelMatrix.rotate(ModelMatrix, radians(0.1f), Vec3(0.f, 0.f, 1.f));
        ModelMatrix = ModelMatrix.scale(ModelMatrix, Vec3(1.f));
        glUniformMatrix4fv(glGetUniformLocation(core_program, "ModelMatrix"), 1, GL_FALSE, ModelMatrix.m);

        // BIND VAO
        glBindVertexArray(VAO);
        
        // DRAW
        glPolygonMode(GL_FRONT_AND_BACK, DRAW_MODE);
        glDrawElements(GL_TRIANGLES, nrOfIndices, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glFlush();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    glDeleteProgram(core_program);

    std::cout << "Exit" << std::endl;
    return 0;
}