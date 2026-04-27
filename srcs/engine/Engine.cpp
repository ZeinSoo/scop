#include "Engine.hpp"

#include "Logger.hpp"

// Static function
void Engine::framebuffer_resize_callback(GLFWwindow *window, int fbW, int fbH) {
    (void)window;
    glViewport(0, 0, fbW, fbH);
}

int Engine::getWindowShouldClose() const {
    return glfwWindowShouldClose(this->window);
}

void Engine::setWindowStatus() {
    glfwSetWindowShouldClose(this->window, GLFW_TRUE);
}

void Engine::render() {
    
    //CLEAR
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    this->updateUniforms();

    // USE PROGRAM
    Shader* shader = this->shaders[0];
    shader->use();

    Material* defaultMaterial = (!this->materials.empty() ? this->materials[1] : nullptr);
    Texture* defaultTexture = nullptr;
    for (Mesh *mesh : this->meshes) {
        if (!mesh) continue;

        // Material selection (per-mesh)
        Material* material = defaultMaterial;
        const std::string& materialName = mesh->getMaterialName();
        if (!materialName.empty()) {
            Material* found = this->findMaterialByName(materialName);
            if (found) material = found;
        }
        if (material) {
            material->sendToShader(*shader);
        }

        // Debug toggles (per-mesh)
        shader->set1i(mesh->getDebugLight() ? 1 : 0, "u_debugLight");
        shader->set1i(mesh->getRenderMode(), "u_renderMode");
        
        // Texture selection (per-mesh). Bind to the unit the material expects.
        const GLint diffuseUnit = (material ? material->getDiffuseTexUnit() : 0);
        Texture* texture = defaultTexture;
        const std::string textureName = mesh->getTextureName();
        if (!textureName.empty()) {
            Texture* found = this->findTextureByName(textureName);
            if (found) texture = found;
        }

        if (texture && texture->getID()) {
            texture->bind(diffuseUnit);
        } else {
            glBindTextureUnit(diffuseUnit, 0);
        }

        // Draw mode (per-mesh)
        if (mesh->getCullingMode()) {
            glEnable(GL_CULL_FACE);
        } else {
            glDisable(GL_CULL_FACE);
        }
        mesh->render(shader, mesh->getDrawMode());
    }
    
    // END DRAW
    glfwSwapBuffers(window);
    glFlush();

    // Reset
    // glBindVertexArray(0);
    // glUseProgram(0);
    // glActiveTexture(0);
    // glBindTexture(GL_TEXTURE_2D, GL_TEXTURE0);
}
