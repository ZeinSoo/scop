#pragma once

#include "libs.hpp"

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

        // Lights
        std::vector<Vec3*> lightsPositions;

        // Init functions
        void initGLFW();
        void initWindow(const char *title,
            bool resizable
        );
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

    public:
        Engine(
            const char *title,
            const int width, const int height,
            const int GL_VERSION_MAJOR, const int GL_VERSION_MINOR,
            bool resizable);

        virtual ~Engine();
        
        // Static function
        static void framebuffer_resize_callback(GLFWwindow *window, int fbW, int fbH);
        static void updateInput(GLFWwindow *window);
        static void updateTransformation(GLFWwindow *window, Mesh &mesh);

        // Getters
        int getWindowShouldClose() const; 

        // Setters
        void setWindowStatus();

        // Main loop functions
        void update();
        void render();
};