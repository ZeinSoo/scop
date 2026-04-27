#include "Engine.hpp"
#include "Logger.hpp"

void Engine::updateUniforms() {
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

void Engine::enqueueMainThreadTask(const std::function<void(Engine&)>& task) {
    std::lock_guard<std::mutex> lock(this->mainThreadTasksMutex);
    this->mainThreadTasks.push(task);
}

void Engine::drainMainThreadTasks() {
    std::queue<std::function<void(Engine&)>> local;
    {
        std::lock_guard<std::mutex> lock(this->mainThreadTasksMutex);
        local.swap(this->mainThreadTasks);
    }
    while (!local.empty()) {
        local.front()(*this);
        local.pop();
    }
}

void Engine::updateInput(GLFWwindow *window, std::vector<Mesh*> meshes) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        for (Mesh* mesh : meshes)
            mesh->setDrawMode(GL_FILL);
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        for (Mesh* mesh : meshes)
            mesh->setDrawMode(GL_LINE);
    }
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
        for (Mesh* mesh : meshes)
            mesh->setDrawMode(GL_POINT);
    }

    // Change render mode with M key or ; 
    static int mWasDown = GLFW_RELEASE;
    const int mIsDown = glfwGetKey(window, GLFW_KEY_M);
    if (mIsDown == GLFW_PRESS && mWasDown == GLFW_RELEASE) {
        for (Mesh* mesh : meshes) {
            mesh->setRenderMode(mesh->getRenderMode() == 0 ? 1 : 0);
        }
    }
    mWasDown = mIsDown;

    // Toggle light mode with L key
    static int lWasDown = GLFW_RELEASE;
    const int lIsDown = glfwGetKey(window, GLFW_KEY_L);
    if (lIsDown == GLFW_PRESS && lWasDown == GLFW_RELEASE) {
        for (Mesh* mesh : meshes) {
            mesh->setDebugLight(!mesh->getDebugLight());
        }
    }
    lWasDown = lIsDown;

    // Toggle culling with C key
    static int cWasDown = GLFW_RELEASE;
    const int cIsDown = glfwGetKey(window, GLFW_KEY_C);
    if (cIsDown == GLFW_PRESS && cWasDown == GLFW_RELEASE) {
        for (Mesh* mesh : meshes) {
            mesh->setCullingMode(!mesh->getCullingMode());
        }
    }
    cWasDown = cIsDown;
}

void Engine::updateTransformation(GLFWwindow *window, Mesh &mesh) {
    // Translation with WASD and QE
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
    
    // Reset with R
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        mesh.setPosition(Vec3(0.f));
        mesh.setRotation(Vec3(0.f));
        mesh.setScale(Vec3(1.f));
    }
}

void Engine::update() {
    // UPDATE INPUT
    glfwPollEvents();
    updateInput(this->window, this->selectedMeshs);

    // Apply any tasks coming from the console menu
    this->drainMainThreadTasks();

    for (Mesh *mesh : this->selectedMeshs)
        updateTransformation(this->window, *mesh);
}
