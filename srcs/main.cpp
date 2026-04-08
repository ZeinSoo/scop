//valgrind --leak-check=full --show-leak-kinds=definite,indirect --suppressions=valgrind.supp ./scop

#include "Engine.hpp"

#include "MatrixStack.hpp"

GLenum DRAW_MODE;
bool DEBUG_TEXTURE = false;
bool DEBUG_COLOR = false;
bool CULLING = false;

int main() {
    
    std::vector<Vertex> temp;
    temp = loadObj("resources_intra/teapot.obj");

    Engine engine("Scop", 1600, 1000, 4, 6, true);    

    // RENDER LOOP
    while (!engine.getWindowShouldClose()) {
        engine.update();
        engine.render();
    }

    std::cout << "Exit" << std::endl;
    return 0;
}
