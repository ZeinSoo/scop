//valgrind --leak-check=full --show-leak-kinds=definite,indirect --suppressions=valgrind.supp ./scop

#include "libs.hpp"

// SQUARE
Vertex vertices[] = {
    Vec3(-0.5f, 0.5f, 0.0f), Vec3(1.0f, 1.0f, 1.0f), Vec2(0.0f, 1.0f), Vec3(0.f, 0.f, 1.f),
    Vec3(-0.5f, -0.5f, 0.0f), Vec3(0.0f, 0.0f, 1.0f), Vec2(0.0f, 0.0f), Vec3(0.f, 0.f, 1.f),
    Vec3(0.5f, -0.5f, 0.0f), Vec3(1.0f, 0.0f, 0.0f), Vec2(1.0f, 0.0f), Vec3(0.f, 0.f, 1.f),
    Vec3(0.5f, 0.5f, 0.0f), Vec3(1.0f, 1.0f, 1.0f), Vec2(1.0f, 1.0f), Vec3(0.f, 0.f, 1.f)
};

GLuint indices[] = {
    0, 1, 2,
    0, 2, 3,
};


// PYRAMID 
// Vertex vertices[] = {
//     //Position                  //Color                  //TexCoords              //Normal
//     Vec3(-0.5f, 0.f, 0.5f),     Vec3(1.f, 0.f, 0.f),    Vec2(0.0f, 0.0f),       Vec3(0.f, 0.f, 1.f),                                          
//     Vec3(-0.5f, 0.f, -0.5f),    Vec3(0.f, 1.f, 0.f),    Vec2(0.0f, 1.0f),       Vec3(0.f, 0.f, 1.f),
//     Vec3(0.5f, 0.f, -0.5f),     Vec3(0.f, 0.f, 1.f),    Vec2(1.0f, 1.0f),       Vec3(0.f, 0.f, 1.f),
//     Vec3(0.5f, 0.f, 0.5f),      Vec3(1.f, 0.f, 1.f),    Vec2(1.0f, 0.0f),       Vec3(0.f, 0.f, 1.f),
//     Vec3(0.f, 0.5f, 0.f),       Vec3(1.f, 1.f, 1.f),    Vec2(0.5f, 0.5f),       Vec3(0.f, 0.f, 1.f)
// };

// GLuint indices[] = {
//     4, 1, 0,
//     4, 2, 1,
//     4, 3, 2,
//     4, 0, 3,
//     0, 1, 2,
//     0, 2, 3
// };

// SCENE FOR LIGHTING TESTS
// Vertex vertices[] = {
//     Vec3(-0.5f, 0.f, 0.f), Vec3(0.f, 0.f, 0.f), Vec2(0.0f, 1.0f), Vec3(0.f, 0.f, 1.f),
//     Vec3(-0.5f, 0.f, -0.5f), Vec3(0.f, 0.f, 0.f), Vec2(0.0f, 1.0f), Vec3(0.f, 0.f, 1.f),
//     Vec3(-0.5f, 0.5f, -0.5f), Vec3(0.f, 0.f, 0.f), Vec2(0.0f, 1.0f), Vec3(0.f, 0.f, 1.f),
//     Vec3(-0.5f, 0.5f, 0.f), Vec3(0.f, 0.f, 0.f), Vec2(0.0f, 1.0f), Vec3(0.f, 0.f, 1.f),
//     Vec3(0.5f, 0.f, 0.f), Vec3(0.f, 0.f, 0.f), Vec2(0.0f, 1.0f), Vec3(0.f, 0.f, 1.f),
//     Vec3(0.5f, 0.f, -0.5f), Vec3(0.f, 0.f, 0.f), Vec2(0.0f, 1.0f), Vec3(0.f, 0.f, 1.f),
//     Vec3(0.5f, 0.5f, -0.5f), Vec3(0.f, 0.f, 0.f), Vec2(0.0f, 1.0f), Vec3(0.f, 0.f, 1.f),
//     Vec3(0.5f, 0.5f, 0.f), Vec3(0.f, 0.f, 0.f), Vec2(0.0f, 1.0f), Vec3(0.f, 0.f, 1.f),
// };

// GLuint indices[] = {
//     3, 0, 1,
//     3, 1, 2, 
//     2, 5, 6,
//     2, 1, 5,
//     6, 4, 7,
//     6, 5, 4,
//     4, 1, 0,
//     4, 5, 1,
//     7, 2, 6,
//     7, 3, 2,
// };

// CUBE
// Vertex vertices[] = {
//     Vec3(-0.5f, -0.5f, 0.5f), Vec3(1.0f, 0.0f, 0.0f), Vec2(0.0f, 0.0f),Vec3(0.f, 0.f, 1.f), // 0
//     Vec3(0.5f, -0.5f, 0.5f), Vec3(0.0f, 1.0f, 0.0f), Vec2(1.0f, 0.0f),Vec3(0.f, 0.f, 1.f),  // 1
//     Vec3(0.5f, 0.5f, 0.5f), Vec3(0.0f, 0.0f, 1.0f), Vec2(1.0f, 1.0f), Vec3(0.f, 0.f, 1.f),  // 2
//     Vec3(-0.5f, 0.5f, 0.5f), Vec3(1.0f, 1.0f, 0.0f), Vec2(0.0f, 1.0f),Vec3(0.f, 0.f, 1.f),  // 3
//     Vec3(-0.5f, -0.5f, -0.5f), Vec3(1.0f, 0.0f, 1.0f), Vec2(0.0f, 0.0f), Vec3(0.f, 0.f, 1.f),// 4
//     Vec3(0.5f, -0.5f, -0.5f), Vec3(0.0f, 1.0f, 1.0f), Vec2(1.0f, 0.0f), Vec3(0.f, 0.f, 1.f), // 5
//     Vec3(0.5f, 0.5f, -0.5f), Vec3(1.0f, 1.0f, 1.0f), Vec2(1.0f, 1.0f), Vec3(0.f, 0.f, 1.f),  // 6
//     Vec3(-0.5f, 0.5f, -0.5f), Vec3(0.2f, 0.2f, 0.2f), Vec2(0.0f, 1.0f), Vec3(0.f, 0.f, 1.f)   // 7
// };

// GLuint indices[] = {
//     // Front (+Z)
//     0, 1, 2,
//     0, 2, 3,
//     4, 6, 5,
//     4, 7, 6,
//     4, 0, 3,
//     4, 3, 7,
//     1, 5, 6,
//     1, 6, 2,
//     3, 2, 6,
//     3, 6, 7,
//     4, 5, 1,
//     4, 1, 0
// };

// PAVE DROIT 
// Vertex vertices[] = {
//     Vec3(-0.5f, -0.25f, 0.75f), Vec3(1.0f, 0.0f, 0.0f), Vec2(0.0f, 0.0f), Vec3(0.f, 0.f, 1.f),
//     Vec3(0.5f, -0.25f, 0.75f), Vec3(0.0f, 1.0f, 0.0f), Vec2(1.0f, 0.0f),  Vec3(0.f, 0.f, 1.f),
//     Vec3(0.5f, 0.25f, 0.75f), Vec3(0.0f, 0.0f, 1.0f), Vec2(1.0f, 1.0f),   Vec3(0.f, 0.f, 1.f),
//     Vec3(-0.5f, 0.25f, 0.75f), Vec3(1.0f, 1.0f, 0.0f), Vec2(0.0f, 1.0f),  Vec3(0.f, 0.f, 1.f),
//     Vec3(-0.5f, -0.25f, -0.75f), Vec3(1.0f, 0.0f, 1.0f), Vec2(0.0f, 0.0f),Vec3(0.f, 0.f, 1.f),
//     Vec3(0.5f, -0.25f, -0.75f), Vec3(0.0f, 1.0f, 1.0f), Vec2(1.0f, 0.0f), Vec3(0.f, 0.f, 1.f),
//     Vec3(0.5f, 0.25f, -0.75f), Vec3(1.0f, 1.0f, 1.0f), Vec2(1.0f, 1.0f),  Vec3(0.f, 0.f, 1.f),
//     Vec3(-0.5f, 0.25f, -0.75f), Vec3(0.2f, 0.2f, 0.2f), Vec2(0.0f, 1.0f),  Vec3(0.f, 0.f, 1.f)
// };

// GLuint indices[] = {
//     // Front (+Z)
//     0, 1, 2,
//     0, 2, 3,
//     // Back (-Z)
//     4, 6, 5,
//     4, 7, 6,
//     // Left (-X)
//     4, 0, 3,
//     4, 3, 7,
//     // Right (+X)
//     1, 5, 6,
//     1, 6, 2,
//     // Top (+Y)
//     3, 2, 6,
//     3, 6, 7,
//     // Bottom (-Y)
//     4, 5, 1,
//     4, 1, 0
// };

unsigned nrOfVertices = sizeof(vertices) / sizeof(Vertex);

unsigned nrOfIndices = sizeof(indices) / sizeof(GLuint);

GLenum DRAW_MODE;
bool DEBUG_TEXTURE = false;

void updateInput(GLFWwindow *window) {
    static int tWasDown = GLFW_RELEASE;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        DRAW_MODE = GL_FILL;
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        DRAW_MODE = GL_LINE;
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
        DRAW_MODE = GL_POINT;

    const int tIsDown = glfwGetKey(window, GLFW_KEY_T);
    if (tIsDown == GLFW_PRESS && tWasDown == GLFW_RELEASE) {
        DEBUG_TEXTURE = !DEBUG_TEXTURE;
        std::cout << "DEBUG_TEXTURE: " << (DEBUG_TEXTURE ? "ON" : "OFF") << std::endl;
    }
    tWasDown = tIsDown;
}

void updateTransformation(GLFWwindow *window, Vec3 &position, Vec3 &rotation, Vec3 &scale) {

    // Translation with WASD and QE
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            position.z -= 0.01f;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            position.z += 0.01f;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            position.x -= 0.01f;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            position.x += 0.01f;;
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
            position.y -= 0.01f;;
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
            position.y += 0.01f;;
    }

    // Rotation with arrow keys and page up/down
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        rotation.x += 1.f;;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        rotation.x -= 1.f;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        rotation.y += 1.f;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        rotation.y -= 1.f;;
    if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)
        rotation.z += 1.f;;
    if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)
        rotation.z -= 1.f;
    
    // Scaling with Z and X
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
        scale += 0.1f;
    }
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
        scale -= 0.1f;
    }
    
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        position = Vec3(0.f);
        rotation = Vec3(0.f);
        scale = Vec3(1.f);
    }

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
    
    std::vector<Vertex> temp;
    temp = loadObj("resources_intra/teapot2.obj");

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

    glfwMakeContextCurrent(window);

    glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
    glViewport(0, 0, framebufferWidth, framebufferHeight);

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

    Shader core_program("resources/vertex_core.glsl", "resources/fragment_core.glsl", "");
    // GLuint core_program;
    // if (!loadShaders(core_program)) {
    //     glfwDestroyWindow(window);
    //     glfwTerminate();
    //     return -1;
    // }
    
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
    // glBufferData(GL_ARRAY_BUFFER, temp.size() * sizeof(Vertex), temp.data(), GL_STATIC_DRAW);

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

    // Normal
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(3);

    // BIND VAO 0
    glBindVertexArray(0);

    // TEXTURE INIT
    Texture texture0("images/wall.bmp", GL_TEXTURE_2D, 0);
    Texture texture1("images/chat.bmp", GL_TEXTURE_2D, 1);

    // INIT Model Matrix

    Vec3 position(0.f);
    Vec3 rotation(0.f);
    Vec3 scale(1.f);

    Mat4 ModelMatrix(1.f);
    ModelMatrix = ModelMatrix.translate(ModelMatrix, position);
    ModelMatrix = ModelMatrix.rotate(ModelMatrix, radians(rotation.x), Vec3(1.f, 0.f, 0.f));
    ModelMatrix = ModelMatrix.rotate(ModelMatrix, radians(rotation.y), Vec3(0.f, 1.f, 0.f));
    ModelMatrix = ModelMatrix.rotate(ModelMatrix, radians(rotation.z), Vec3(0.f, 0.f, 1.f));
    ModelMatrix = ModelMatrix.scale(ModelMatrix, scale);

    // INIT View Matrix
    Vec3 camPos(0.f, 0.f, 4.f);
    Vec3 worldUp(0.f, 1.f, 0.f);
    Vec3 camFront(0.f, 0.f, -1.f);
    Mat4 ViewMatrix(1.f);
    ViewMatrix = ViewMatrix.lookAt(camPos, camPos + camFront, worldUp);

    // INIT Projection Matrix
    float fov = 90.f;
    float nearPlane = 0.1f;
    float farPlane = 1000.f;

    Mat4 ProjectionMatrix(1.f);
    ProjectionMatrix = ProjectionMatrix.perspective(
        radians(fov), 
        static_cast<float>(framebufferWidth) / static_cast<float>(framebufferHeight), 
        nearPlane, 
        farPlane
    );

    // glm::mat4 glmModelMatrix(1.f);
    // glmModelMatrix = glm::translate(glmModelMatrix, glm::vec3(0.f, 0.f, 0.f));
    // glmModelMatrix = glm::rotate(glmModelMatrix, radians(0.f), glm::vec3(1.f, 0.f, 0.f));
    // glmModelMatrix = glm::rotate(glmModelMatrix, radians(0.f), glm::vec3(0.f, 1.f, 0.f));
    // glmModelMatrix = glm::rotate(glmModelMatrix, radians(0.f), glm::vec3(0.f, 0.f, 1.f));
    // glmModelMatrix = glm::scale(glmModelMatrix, glm::vec3(1.f));

    // glm::vec3 glmCamPos(0.f);
    // glm::vec3 glmWorldUp(0.f, 1.f, 0.f);
    // glm::vec3 glmCamFront(0.f, 0.f, -1.f);
    // glm::mat4 glmViewMatrix(1.f);
    // glmViewMatrix = glm::lookAt(glmCamPos, glmCamPos + glmCamFront, glmWorldUp);
    // glm::vec3 glmCenter = glmCamPos + glmCamFront;
    // std::cout << glmViewMatrix << std::endl;

    // LIGHTS
    Vec3 lightPos0(0.f, 0.f, 3.f);

    // INIT UNIFORMS

    core_program.setMat4f(ModelMatrix, "ModelMatrix");
    core_program.setMat4f(ViewMatrix, "ViewMatrix");
    core_program.setMat4f(ProjectionMatrix, "ProjectionMatrix");
    
    core_program.setVec3f(lightPos0, "lightPos0");
    core_program.setVec3f(camPos, "cameraPos");


    // RENDER LOOP
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // UPDATE INPUT
        updateInput(window);
        updateTransformation(window, position, rotation, scale);

        //CLEAR
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        // USE PROGRAM
        core_program.use();
        
        // UPDATE UNIFORMS
        core_program.set1i(texture0.getTextureUnit(), "texture0");
        core_program.set1i(DEBUG_TEXTURE ? 1 : 0, "u_debugTexture");
        // glUniform1i(glGetUniformLocation(core_program.getId(), "texture0"), 0);
        // glUniform1i(glGetUniformLocation(core_program.getId(), "u_debugTexture"), DEBUG_TEXTURE ? 1 : 0);
        
        // ACTIVATE AND BIND TEXTURE
        texture0.bind();
        texture1.bind();

        // Move, Rotate, Scale
        // position.z -= 0.01f;
        // rotation.y += 2.f;

        ModelMatrix = Mat4(1.f);
        ModelMatrix = ModelMatrix.translate(ModelMatrix, position);
        ModelMatrix = ModelMatrix.rotate(ModelMatrix, radians(rotation.x), Vec3(1.f, 0.f, 0.f));
        ModelMatrix = ModelMatrix.rotate(ModelMatrix, radians(rotation.y), Vec3(0.f, 1.f, 0.f));
        ModelMatrix = ModelMatrix.rotate(ModelMatrix, radians(rotation.z), Vec3(0.f, 0.f, 1.f));
        ModelMatrix = ModelMatrix.scale(ModelMatrix, scale);
        core_program.setMat4f(ModelMatrix, "ModelMatrix");
        // BIND VAO
        glBindVertexArray(VAO);
        
        // DRAW
        glPolygonMode(GL_FRONT_AND_BACK, DRAW_MODE);
        // std::cout << temp.size() << std::endl;
        glDrawElements(GL_TRIANGLES, nrOfIndices, GL_UNSIGNED_INT, 0);
        // glDrawArrays(GL_TRIANGLES, 0, temp.size());

        glfwSwapBuffers(window);
        glFlush();
    }

    // for (long unsigned int i = 0; i < temp.size(); i++) {
    //     std::cout << temp.at(i).position << std::endl;
    // }

    glfwDestroyWindow(window);
    glfwTerminate();

    std::cout << "Exit" << std::endl;
    return 0;
}