//valgrind --leak-check=full --show-leak-kinds=definite,indirect --suppressions=valgrind.supp ./scop

#include "Engine.hpp"

#include "ConsoleMenu.hpp"

#include "Logger.hpp"

GLenum DRAW_MODE;
bool DEBUG_TEXTURE = false;
bool DEBUG_COLOR = false;
bool CULLING = false;

int main() {
    Engine engine("Scop", 800, 600, 4, 6, true);

    ConsoleMenu menu(engine);
    menu.start();
    
    // RENDER LOOP
    while (!engine.getWindowShouldClose()) {
        engine.update();
        engine.render();
    }

    menu.stop();

    Logger::log("Exit");
    return 0;
}
