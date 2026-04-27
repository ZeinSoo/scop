#include "Engine.hpp"
#include "Logger.hpp"

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

// Init functions

void Engine::initGLFW() {
    if (!glfwInit()) {
        Logger::log("ERROR::MAIN.CPP::GLFW_INIT_FAILED");
        glfwTerminate();
        // return -1;
    }
}

void Engine::initWindow(const char *title, bool resizable) {
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, this->GL_VERSION_MAJOR);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, this->GL_VERSION_MINOR);

        glfwWindowHint(GLFW_RESIZABLE, resizable);

        GLFWwindow *window = glfwCreateWindow(this->WIN_WIDTH, this->WIN_HEIGHT, title, NULL, NULL);
        if (!window) {
            Logger::log("ERROR::MAIN.CPP::GLFW_WINDOW_CREATION_FAILED");
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
        Logger::log("ERROR::MAIN.CPP::GLEW_INIT_FAILED");
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
}

void Engine::initMaterials() {
    this->materials.push_back(new Material(Vec3(2.f), Vec3(0.1f, 0.1f, 0.1f), Vec3(0.5f), 96.f, 1.f, 2, 0, 1));
    this->materials.push_back(new Material(Vec3(0.f), Vec3(0.64f), Vec3(0.5f), 96.078431f, 1.f, 2, 0, 0, "default"));
}

void Engine::initMeshes() {
    try {
        this->addMeshFromObjFile("resources_intra/42.obj", "42");
        Mesh* mesh = this->findMeshByName("42");
        if (mesh)
            this->selectedMeshs.push_back(mesh);
    } catch (const std::exception& ex) {
        Logger::log(std::string("[Engine] initMeshes: failed to load default OBJ: ") + ex.what());
    }
}

void Engine::initLights() {
    this->lightsPositions.push_back(new Vec3(0.f, 0.f, 2.f));
}

void Engine::initUniforms() {
    this->shaders[0]->setMat4f(this->ViewMatrix, "ViewMatrix");
    this->shaders[0]->setMat4f(this->ProjectionMatrix, "ProjectionMatrix");
    
    this->shaders[0]->setVec3f(*this->lightsPositions[0], "lightPos0");
    this->shaders[0]->setVec3f(this->viewData.camPos, "cameraPos");
}
