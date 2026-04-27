#pragma once

#include "libs.hpp"

#include <functional>
#include <mutex>
#include <queue>

class ConsoleMenu;

struct ViewData {
    Vec3 camPos;
    Vec3 worldUp;
    Vec3 camFront;
};

struct ProjectionData {
    float fov;
    float farPlane;
    float nearPlane;
};

class Engine {
    private:
        // Window parameters
        GLFWwindow *window;
        const int WIN_WIDTH;
        const int WIN_HEIGHT;
        int framebufferWidth;
        int framebufferHeight;

        // OpenGL Context
        const int GL_VERSION_MAJOR;
        const int GL_VERSION_MINOR;

        // Matrices
        Mat4 ViewMatrix;
        ViewData viewData;
        Mat4 ProjectionMatrix;
        ProjectionData projectionData;

        // Shaders
        std::vector<Shader*> shaders;

        // Textures
        std::vector<Texture*> textures;

        // Materials
        std::vector<Material*> materials;

        // Meshes
        std::vector<Mesh*> meshes;
        std::vector<Mesh*> selectedMeshs;

        // Lights
        std::vector<Vec3*> lightsPositions;

        // Main-thread tasks (enqueued from console thread)
        std::mutex mainThreadTasksMutex;
        std::queue<std::function<void(Engine&)>> mainThreadTasks;

        // Init functions
        void initGLFW();
        void initWindow(const char *title, bool resizable);
        void initGLEW();
        void initOpenGLOptions();
        void initMatrices();
        void initShaders();
        void initTextures();
        void initMaterials();
        void initMeshes();
        void initLights();
        void initUniforms();

        void updateUniforms();

        void drainMainThreadTasks();

        Mesh *findMeshByName(const std::string &name);
        Material *findMaterialByName(const std::string &name);
        Texture *findTextureByName(const std::string &name);

    public:
        Engine(
            const char *title,
            const int width, const int height,
            const int GL_VERSION_MAJOR, const int GL_VERSION_MINOR,
            bool resizable);

        virtual ~Engine();
        
        // Static function
        static void framebuffer_resize_callback(GLFWwindow *window, int fbW, int fbH);
        static void updateInput(GLFWwindow *window, std::vector<Mesh*> meshes);
        static void updateTransformation(GLFWwindow *window, Mesh &mesh);

        // Getters
        int getWindowShouldClose() const;
        std::vector<Texture*> getTextures() const;
        std::vector<Material*> getMaterials() const;
        std::vector<Mesh*> getMeshes() const;
        std::vector<Vec3> getLightsPositions() const;

        // Setters
        void setWindowStatus();
        void addSelectedMesh(Mesh *mesh);
        void removeSelectedMesh(Mesh *mesh);
        void clearSelectedMeshs();

        // Console -> Engine bridge
        void enqueueMainThreadTask(const std::function<void(Engine&)>& task);
        void addTextureFromFile(const std::string& filePath);
        bool removeTextureWithName(const std::string& name);
        void addMeshFromPrimitive(const Primitive &primitive, const std::string &name = "");
        void addMeshFromObjFile(const std::string& filePath, const std::string& name = "");
        void removeMeshWithName(std::string name);

        // Lights
        void addLight(const Vec3& position);
        bool removeLightAtIndex(size_t index);
        bool setLightPosition(size_t index, const Vec3& position);

        // Main loop functions
        void update();
        void render();
};