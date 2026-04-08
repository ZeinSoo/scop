#include "Engine.hpp"

Engine::Engine(const char *title,
            const int width, const int height,
            const int GL_VERSION_MAJOR, const int GL_VERSION_MINOR,
            bool resizable) 
            : WIN_WIDTH(width), WIN_HEIGHT(height), GL_VERSION_MAJOR(GL_VERSION_MAJOR), GL_VERSION_MINOR(GL_VERSION_MINOR) {
    this->window = nullptr;
    this->framebufferWidth = this->WIN_WIDTH;
    this->framebufferHeight = this->WIN_HEIGHT;

    this->viewData.camPos = Vec3(0.f, 0.f, 4.f);
    this->viewData.worldUp = Vec3(0.f, 1.f, 0.f);
    this->viewData.camFront = Vec3(0.f, 0.f, -1.f);

    this->projectionData.fov = 90.f;
    this->projectionData.nearPlane = 0.1f;
    this->projectionData.farPlane = 1000.f;

    this->initGLFW();
    this->initWindow(title, resizable);
    this->initGLEW();
    this->initOpenGLOptions();
    this->initMatrices();
    this->initShaders();
    this->initTextures();
    this->initMaterials();
    this->initMeshes();
    this->initLights();
    this->initUniforms();
}

Engine::~Engine() {
    glfwDestroyWindow(this->window);
    glfwTerminate();

    for (Shader *shader : this->shaders)
        delete shader;
    for (Texture *texture : this->textures)
        delete texture;
    for (Material *material : this->materials)
        delete material;
    for (Mesh *mesh : this->meshes) 
        delete mesh;
    for (Vec3 *lightPos : this->lightsPositions)
        delete lightPos;
}

// Static function
void Engine::framebuffer_resize_callback(GLFWwindow *window, int fbW, int fbH) {
    (void)window;
    glViewport(0, 0, fbW, fbH);
    std::cout << "In function : " << fbW << "  " << fbH << std::endl;
}

// Init functions

void Engine::initGLFW() {
    if (!glfwInit()) {
        std::cout << "ERROR::MAIN.CPP::GLFW_INIT_FAILED" << std::endl;
        glfwTerminate();
        // return -1;
    }
}

void Engine::initWindow(const char *title,
    bool resizable) {
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, this->GL_VERSION_MAJOR);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, this->GL_VERSION_MINOR);

        glfwWindowHint(GLFW_RESIZABLE, resizable);

        std::cout << this->WIN_WIDTH << "  " << this->WIN_HEIGHT << std::endl;
        GLFWwindow *window = glfwCreateWindow(this->WIN_WIDTH, this->WIN_HEIGHT, title, NULL, NULL);
        if (!window) {
            std::cout << "ERROR::MAIN.CPP::GLFW_WINDOW_CREATION_FAILED" << std::endl;
            glfwTerminate();
            // return -1;
        }
        
        glfwGetFramebufferSize(window, &(this->framebufferWidth), &(framebufferHeight));
        glfwSetFramebufferSizeCallback(window, this->framebuffer_resize_callback);
        
        glfwMakeContextCurrent(window);

        this->window = window;
}

void Engine::initGLEW() {
    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK) {
        std::cout << "ERROR::MAIN.CPP::GLEW_INIT_FAILED" << std::endl;
        glfwTerminate();
        // return -1;
    }
}

void Engine::initOpenGLOptions() {
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // DRAW_MODE = GL_FILL;
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Engine::initMatrices() {
    
    this->ViewMatrix = Mat4(1.f);
    this->ViewMatrix = this->ViewMatrix.lookAt(this->viewData.camPos, this->viewData.camPos + this->viewData.camFront, this->viewData.worldUp);

    this->ProjectionMatrix = Mat4(1.f);
    ProjectionMatrix = ProjectionMatrix.perspective(
        radians(this->projectionData.fov), 
        static_cast<float>(framebufferWidth) / static_cast<float>(framebufferHeight), 
        this->projectionData.nearPlane, 
        this->projectionData.farPlane
    );
}

void Engine::initShaders() {
    this->shaders.push_back(new Shader("shaders/vertex_core.glsl", "shaders/fragment_core.glsl", ""));
}

void Engine::initTextures() {
    this->textures.push_back(new Texture("images/qq-4x.bmp", GL_TEXTURE_2D));
    this->textures.push_back(new Texture("images/qq-4x.bmp", GL_TEXTURE_2D));
}

void Engine::initMaterials() {
    this->materials.push_back(new Material(Vec3(2.f), Vec3(0.1f, 0.1f, 0.1f), Vec3(0.5f), 96.f, 1.f, 2, 0, 1));
    this->materials.push_back(new Material(Vec3(0.f), Vec3(0.64f), Vec3(0.5f), 96.078431f, 1.f, 2, 0, 0));

}

void Engine::initMeshes() {
    std::vector<Vertex> temp = loadObj("resources_intra/42.obj");
    this->meshes.push_back(new Mesh(temp.data(), temp.size(), NULL, 0));
    // this->meshes.push_back(new Mesh(Cube{}));
}

void Engine::initLights() {
    this->lightsPositions.push_back(new Vec3(0.f, 0.f, 1.f));
}

void Engine::initUniforms() {
    this->shaders[0]->setMat4f(this->ViewMatrix, "ViewMatrix");
    this->shaders[0]->setMat4f(this->ProjectionMatrix, "ProjectionMatrix");
    
    this->shaders[0]->setVec3f(*this->lightsPositions[0], "lightPos0");
    this->shaders[0]->setVec3f(this->viewData.camPos, "cameraPos");
}

void Engine::updateUniforms() {
    // this->shaders[0]->set1i(DEBUG_TEXTURE ? 1 : 0, "u_debugTexture");
    // this->shaders[0]->set1i(DEBUG_COLOR ? 1 : 0, "u_debugColor");
    this->materials[0]->sendToShader(*this->shaders[0]);

    // Update projection matrix in case of window resize
    glfwGetFramebufferSize(this->window, &(this->framebufferWidth), &(framebufferHeight));
    this->ProjectionMatrix = this->ProjectionMatrix.perspective(
        radians(this->projectionData.fov), 
        static_cast<float>(framebufferWidth) / static_cast<float>(framebufferHeight), 
        this->projectionData.nearPlane, 
        this->projectionData.farPlane
    );
    this->shaders[0]->setMat4f(this->ProjectionMatrix, "ProjectionMatrix");
}

int Engine::getWindowShouldClose() const {
    return glfwWindowShouldClose(this->window);
}

void Engine::setWindowStatus() {
    glfwSetWindowShouldClose(this->window, GLFW_TRUE);
}

void Engine::updateInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    // if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
    //     DRAW_MODE = GL_FILL;
    // if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
    //     DRAW_MODE = GL_LINE;
    // if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
    //     DRAW_MODE = GL_POINT;
    
    // // Toggle debug texture mode with T key
    // static int tWasDown = GLFW_RELEASE;
    // const int tIsDown = glfwGetKey(window, GLFW_KEY_T);
    // if (tIsDown == GLFW_PRESS && tWasDown == GLFW_RELEASE) {
    //     DEBUG_TEXTURE = !DEBUG_TEXTURE;
    //     DEBUG_COLOR = false;
    //     std::cout << "DEBUG_TEXTURE: " << (DEBUG_TEXTURE ? "ON" : "OFF") << std::endl;
    // }
    // tWasDown = tIsDown;

    // // Toggle color mode with C key
    // static int cWasDown = GLFW_RELEASE;
    // const int cIsDown = glfwGetKey(window, GLFW_KEY_C);
    // if (cIsDown == GLFW_PRESS && cWasDown == GLFW_RELEASE) {
    //     DEBUG_COLOR = !DEBUG_COLOR;
    //     DEBUG_TEXTURE = false;
    //     std::cout << "DEBUG_TEXTURE: " << (DEBUG_COLOR ? "ON" : "OFF") << std::endl;
    // }
    // cWasDown = cIsDown;

    // // Toggle culling with F key
    // static int fWasDown = GLFW_RELEASE;
    // const int fIsDown = glfwGetKey(window, GLFW_KEY_F);
    // if (fIsDown == GLFW_PRESS && fWasDown == GLFW_RELEASE) {
    //     CULLING = !CULLING;
    //     if (CULLING) {
    //         glEnable(GL_CULL_FACE);
    //         std::cout << "CULLING: ON" << std::endl;
    //     } else {
    //         glDisable(GL_CULL_FACE);
    //         std::cout << "CULLING: OFF" << std::endl;
    //     }
    // }
    // fWasDown = fIsDown;
    // // std::cout << "\033[2J\033[H";
}

void Engine::updateTransformation(GLFWwindow *window, Mesh &mesh) {
    // Translation with WASD and QE
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            mesh.move(Vec3(0.f, 0.f, -0.01f));
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            mesh.move(Vec3(0.f, 0.f, 0.01f));
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            mesh.move(Vec3(-0.01f, 0.f, 0.f));
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            mesh.move(Vec3(0.01f, 0.f, 0.f));
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
            mesh.move(Vec3(0.f, -0.01f, 0.f));
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
            mesh.move(Vec3(0.f, 0.01f, 0.f));
    }

    // Rotation with arrow keys and page up/down
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        mesh.rotate(Vec3(1.f, 0.f, 0.f));
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        mesh.rotate(Vec3(-1.f, 0.f, 0.f));
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        mesh.rotate(Vec3(0.f, 1.f, 0.f));
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        mesh.rotate(Vec3(0.f, -1.f, 0.f));
    if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)
        mesh.rotate(Vec3(0.f, 0.f, 1.f));
    if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)
        mesh.rotate(Vec3(0.f, 0.f, -1.f));

    // Scaling with Z and X
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
        mesh.scaleUp(Vec3(0.1f));
    }
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
        mesh.scaleUp(Vec3(-0.1f));
    }
    
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        mesh.setPosition(Vec3(0.f));
        mesh.setRotation(Vec3(0.f));
        mesh.setScale(Vec3(1.f));
    }
}

void Engine::update() {
    // UPDATE INPUT

    std::cout << "HERE BEFORE : " << this->framebufferWidth << "  " << this->framebufferHeight << std::endl;
    glfwPollEvents();
    std::cout << "HERE AFTER : " << this->framebufferWidth << "  " << this->framebufferHeight << std::endl;
    updateInput(this->window);

    for (Mesh *mesh : this->meshes)
        updateTransformation(this->window, *mesh);
}

void Engine::render() {
    // updateTransformation(window, teapot);

    //CLEAR
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    this->updateUniforms();

    // USE PROGRAM
    this->shaders[0]->use();

    // ACTIVATE AND BIND TEXTURE
    // texture0.bind();
    this->textures[1]->bind(1);
    this->textures[0]->bind(0);
    
    
    // teapot.setScale(Vec3(1.f));
    for (Mesh *mesh : this->meshes)
        mesh->render(this->shaders[0], GL_FILL);
    // this->meshes[0]->render(this->shaders[0], GL_FILL);
    
    // END DRAW
    glfwSwapBuffers(window);
    glFlush();

    // Reset
    // glBindVertexArray(0);
    // glUseProgram(0);
    // glActiveTexture(0);
    // glBindTexture(GL_TEXTURE_2D, GL_TEXTURE0);
}