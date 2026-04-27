#include "ConsoleMenu.hpp"

#include "Engine.hpp"
#include "Primitives.hpp"
#include "Logger.hpp"

#include <poll.h>
#include <unistd.h>
#include <fcntl.h>

#include <cerrno>
#include <iostream>
#include <future>
#include <chrono>
#include <sstream>
#include <utility>

static bool parseVec3(const std::string& s, Vec3& out) {
    std::istringstream iss(s);
    float x = 0.f;
    float y = 0.f;
    float z = 0.f;
    if (!(iss >> x >> y >> z)) return false;
    out = Vec3(x, y, z);
    return true;
}

static std::string basenameOfPath(const std::string& path) {
    const size_t pos = path.find_last_of("/\\");
    if (pos == std::string::npos) return path;
    return path.substr(pos + 1);
}

ConsoleMenu::ConsoleMenu(Engine& engine)
    : engine(engine), running(false), dirty(true), awaitingLine(false), allowEmptyLineInPrompt(false) {
    promptVisible = false;
    buildMenus();
}

ConsoleMenu::~ConsoleMenu() {
    stop();
}

void ConsoleMenu::start() {
    if (running.load()) return;
    running = true;
    worker = std::thread(&ConsoleMenu::threadMain, this);
}

void ConsoleMenu::stop() {
    running = false;
    if (worker.joinable()) {
    worker.join();
    }
}

void ConsoleMenu::buildMenus() {
    home.reset(new MenuNode());
    textureManager.reset(new MenuNode());
    lightManager.reset(new MenuNode());
    meshManager.reset(new MenuNode());
    addMesh.reset(new MenuNode());
    meshOptions.reset(new MenuNode());

    home->title = "Home";
    textureManager->title = "Texture Manager";
    lightManager->title = "Light Manager";
    meshManager->title = "Mesh Manager";
    addMesh->title = "Add Mesh";
    meshOptions->title = "Mesh Options";

    stack.clear();
    stack.push_back(home.get());

    editedMeshName.clear();

    auto manual = [this]() {
        std::cout << "Manual: choose a number then press Enter.\n";
        std::cout << "- Return is always [0] inside submenus\n";
        std::cout << "- Mesh keys affect the currently selected mesh\n";
        dirty = true;
    };

    auto snapshotTextureNames = [this]() -> std::vector<std::string> {
        auto p = std::make_shared<std::promise<std::vector<std::string>>>();
        auto f = p->get_future();
        engine.enqueueMainThreadTask([p](Engine& e) {
            std::vector<std::string> names;
            for (Texture* t : e.getTextures()) {
                if (t) names.push_back(t->getName());
            }
            p->set_value(names);
        });
        return f.get();
    };

    auto snapshotMaterialNames = [this]() -> std::vector<std::string> {
        auto p = std::make_shared<std::promise<std::vector<std::string>>>();
        auto f = p->get_future();
        engine.enqueueMainThreadTask([p](Engine& e) {
            std::vector<std::string> names;
            for (Material* m : e.getMaterials()) {
                if (m) names.push_back(m->getName());
            }
            p->set_value(names);
        });
        return f.get();
    };

    auto snapshotMeshNames = [this]() -> std::vector<std::string> {
        auto p = std::make_shared<std::promise<std::vector<std::string>>>();
        auto f = p->get_future();
        engine.enqueueMainThreadTask([p](Engine& e) {
            std::vector<std::string> names;
            for (Mesh* m : e.getMeshes()) {
                if (m) names.push_back(m->getName());
            }
            p->set_value(names);
        });
        return f.get();
    };

    auto snapshotLights = [this]() -> std::vector<Vec3> {
        auto p = std::make_shared<std::promise<std::vector<Vec3>>>();
        auto f = p->get_future();
        engine.enqueueMainThreadTask([p](Engine& e) {
            p->set_value(e.getLightsPositions());
        });
        return f.get();
    };

    home->items = {
        {"Manual", manual},
        {"Texture Manager", [this]() { push(textureManager.get()); }},
        {"Light Manager", [this]() { push(lightManager.get()); }},
        {"Mesh Manager", [this]() { push(meshManager.get()); }},
        {"Quit", [this]() {
           engine.setWindowStatus();
           running = false;
         }},
    };

    textureManager->items = {
        {"Return", [this]() { pop(); }},
        {"Add Texture", [this, snapshotTextureNames]() {
            const std::vector<std::string> textures = snapshotTextureNames();
            if (textures.empty()) {
                std::cout << "No textures loaded.\n";
            } else {
                std::cout << "\nTextures List:\n";
                for (size_t i = 0; i < textures.size(); ++i)
                    std::cout << "[" << i << "] " << textures[i] << "\n";
            }

            awaitingLine = true;
            promptText = "Texture path: ";
            lineHandler = [this](const std::string& rawPath) {
                const std::string path = rawPath;
                if (path.empty()) return;
                engine.enqueueMainThreadTask([path](Engine& e) {
                    try {
                        e.addTextureFromFile(path);
                    } catch (const std::exception& ex) {
                        Logger::log(std::string("[ConsoleMenu] Failed to add texture: ") + ex.what());
                    }
                });
            };
            dirty = true;
        }},
        {"Delete Texture", [this, snapshotTextureNames]() {
            const std::vector<std::string> textures = snapshotTextureNames();
            if (textures.empty()) {
                std::cout << "No textures to delete.\n";
                dirty = true;
                return;
            }
            std::cout << "\nTextures List:\n";
            for (size_t i = 0; i < textures.size(); ++i)
                std::cout << "[" << i << "] " << textures[i] << "\n";

            awaitingLine = true;
            promptText = "Enter texture number: ";
            lineHandler = [this, textures](const std::string& line) {
                size_t idx = 0;
                if (!parseIndex(line, idx) || idx >= textures.size()) {
                    std::cout << "Invalid texture number.\n";
                    dirty = true;
                    return;
                }
                const std::string name = textures[idx];
                engine.enqueueMainThreadTask([name](Engine& e) {
                    e.removeTextureWithName(name);
                });
            };
            dirty = true;
        }},
    };

    lightManager->items = {
        {"Return", [this]() { pop(); }},
        {"Add Light", [this, snapshotLights]() {
            const std::vector<Vec3> lights = snapshotLights();
            if (lights.empty()) {
                std::cout << "No lights currently.\n";
            } else {
                std::cout << "\nLights List:\n";
                for (size_t i = 0; i < lights.size(); ++i) {
                    std::cout << "[" << i << "] (" << lights[i].x << ", " << lights[i].y << ", " << lights[i].z << ")\n";
                }
            }

            awaitingLine = true;
            allowEmptyLineInPrompt = true;
            promptText = "New light position (x y z) (empty = default): ";
            lineHandler = [this](const std::string& line) {
                Vec3 pos(0.f, 0.f, 2.f);
                if (!line.empty() && !parseVec3(line, pos)) {
                    std::cout << "Invalid position. Expected: x y z\n";
                    dirty = true;
                    return;
                }
                engine.enqueueMainThreadTask([pos](Engine& e) {
                    e.addLight(pos);
                });
            };
            dirty = true;
        }},
        {"Delete Light", [this, snapshotLights]() {
            const std::vector<Vec3> lights = snapshotLights();
            if (lights.empty()) {
                std::cout << "No lights to delete.\n";
                dirty = true;
                return;
            }
            std::cout << "\nLights List:\n";
            for (size_t i = 0; i < lights.size(); ++i) {
                std::cout << "[" << i << "] (" << lights[i].x << ", " << lights[i].y << ", " << lights[i].z << ")\n";
            }

            awaitingLine = true;
            promptText = "Enter light number: ";
            lineHandler = [this, lights](const std::string& line) {
                size_t idx = 0;
                if (!parseIndex(line, idx) || idx >= lights.size()) {
                    std::cout << "Invalid light number.\n";
                    dirty = true;
                    return;
                }
                engine.enqueueMainThreadTask([idx](Engine& e) {
                    e.removeLightAtIndex(idx);
                });
            };
            dirty = true;
        }},
        {"Move Light", [this, snapshotLights]() {
            const std::vector<Vec3> lights = snapshotLights();
            if (lights.empty()) {
                std::cout << "No lights to move.\n";
                dirty = true;
                return;
            }
            std::cout << "\nLights List:\n";
            for (size_t i = 0; i < lights.size(); ++i) {
                std::cout << "[" << i << "] (" << lights[i].x << ", " << lights[i].y << ", " << lights[i].z << ")\n";
            }

            awaitingLine = true;
            promptText = "Enter light number: ";
            lineHandler = [this, lights](const std::string& line) {
                size_t idx = 0;
                if (!parseIndex(line, idx) || idx >= lights.size()) {
                    std::cout << "Invalid light number.\n";
                    dirty = true;
                    return;
                }

                awaitingLine = true;
                promptText = "New position (x y z): ";
                lineHandler = [this, idx](const std::string& posLine) {
                    Vec3 pos;
                    if (!parseVec3(posLine, pos)) {
                        std::cout << "Invalid position. Expected: x y z\n";
                        dirty = true;
                        return;
                    }
                    engine.enqueueMainThreadTask([idx, pos](Engine& e) {
                        e.setLightPosition(idx, pos);
                    });
                };
                dirty = true;
            };
            dirty = true;
        }},
    };

    meshManager->items = {
        {"Return", [this]() { pop(); }},
        {"Add Mesh", [this]() { push(addMesh.get()); }},
        {"Delete Mesh", [this, snapshotMeshNames]() {
            const std::vector<std::string> meshes = snapshotMeshNames();
            if (meshes.empty()) {
                std::cout << "No meshes to delete.\n";
                dirty = true;
                return;
            }
            std::cout << "\nMeshes List:\n";
            for (size_t i = 0; i < meshes.size(); ++i)
                std::cout << "[" << i << "] " << meshes[i] << "\n";

            awaitingLine = true;
            promptText = "Enter mesh number: ";
            lineHandler = [this, meshes](const std::string& line) {
                size_t idx = 0;
                if (!parseIndex(line, idx) || idx >= meshes.size()) {
                    std::cout << "Invalid mesh number.\n";
                    dirty = true;
                    return;
                }
                const std::string name = meshes[idx];
                engine.enqueueMainThreadTask([name](Engine& e) {
                    e.removeMeshWithName(name);
                });
            };
            dirty = true;
        }},
        {"Edit Mesh", [this, snapshotMeshNames]() {
            const std::vector<std::string> meshes = snapshotMeshNames();
            if (meshes.empty()) {
                std::cout << "No meshes to edit.\n";
                dirty = true;
                return;
            }
            std::cout << "\nMeshes List:\n";
            for (size_t i = 0; i < meshes.size(); ++i)
                std::cout << "[" << i << "] " << meshes[i] << "\n";

            awaitingLine = true;
            promptText = "Enter mesh number: ";
            lineHandler = [this, meshes](const std::string& line) {
                size_t idx = 0;
                if (!parseIndex(line, idx) || idx >= meshes.size()) {
                    std::cout << "Invalid mesh number.\n";
                    dirty = true;
                    return;
                }

                editedMeshName = meshes[idx];
                meshOptions->title = std::string("Mesh Options: ") + editedMeshName;

                const std::string meshName = editedMeshName;
                auto done = std::make_shared<std::promise<void>>();
                std::future<void> doneFuture = done->get_future();
                engine.enqueueMainThreadTask([meshName, done](Engine& e) {
                    e.clearSelectedMeshs();
                    for (Mesh* m : e.getMeshes()) {
                        if (m && m->getName() == meshName) {
                            e.addSelectedMesh(m);
                            Logger::log(std::string("[Engine] Selected mesh: ") + meshName);
                            done->set_value();
                            return;
                        }
                    }
                    Logger::log(std::string("[Engine] Mesh not found: ") + meshName);
                    done->set_value();
                });

                // Ensure the selection log is available before we render the menu,
                // otherwise the menu prints once, then logs print, then menu prints again.
                const std::chrono::milliseconds step(10);
                for (int i = 0; i < 50 && running.load(); ++i) {
                    if (doneFuture.wait_for(step) == std::future_status::ready)
                        break;
                }

                push(meshOptions.get());
            };
            dirty = true;
        }},
    };

    addMesh->items = {
        {"Return", [this]() { pop(); }},
        {"Triangle", [this]() {
            engine.enqueueMainThreadTask([](Engine& e) {
                try {
                    const std::string base = "Triangle";
                    std::string name = base;
                    int suffix = 1;
                    const std::vector<Mesh*> meshes = e.getMeshes();
                    auto exists = [&](const std::string& candidate) {
                        for (Mesh* m : meshes) {
                            if (m && m->getName() == candidate) return true;
                        }
                        return false;
                    };
                    while (exists(name)) name = base + "_" + std::to_string(suffix++);
                    e.addMeshFromPrimitive(Triangle{}, name);
                } catch (const std::exception& ex) {
                    Logger::log(std::string("[ConsoleMenu] Failed to add mesh: ") + ex.what());
                }
            });
        }},
        {"Quad", [this]() {
            engine.enqueueMainThreadTask([](Engine& e) {
                try {
                    const std::string base = "Quad";
                    std::string name = base;
                    int suffix = 1;
                    const std::vector<Mesh*> meshes = e.getMeshes();
                    auto exists = [&](const std::string& candidate) {
                        for (Mesh* m : meshes) {
                            if (m && m->getName() == candidate) return true;
                        }
                        return false;
                    };
                    while (exists(name)) name = base + "_" + std::to_string(suffix++);
                    e.addMeshFromPrimitive(Quad{}, name);
                } catch (const std::exception& ex) {
                    Logger::log(std::string("[ConsoleMenu] Failed to add mesh: ") + ex.what());
                }
            });
        }},
        {"Pyramid", [this]() {
            engine.enqueueMainThreadTask([](Engine& e) {
                try {
                    const std::string base = "Pyramid";
                    std::string name = base;
                    int suffix = 1;
                    const std::vector<Mesh*> meshes = e.getMeshes();
                    auto exists = [&](const std::string& candidate) {
                        for (Mesh* m : meshes) {
                            if (m && m->getName() == candidate) return true;
                        }
                        return false;
                    };
                    while (exists(name)) name = base + "_" + std::to_string(suffix++);
                    e.addMeshFromPrimitive(Pyramid{}, name);
                } catch (const std::exception& ex) {
                    Logger::log(std::string("[ConsoleMenu] Failed to add mesh: ") + ex.what());
                }
            });
        }},
        {"Cube", [this]() {
            engine.enqueueMainThreadTask([](Engine& e) {
                try {
                    const std::string base = "Cube";
                    std::string name = base;
                    int suffix = 1;
                    const std::vector<Mesh*> meshes = e.getMeshes();
                    auto exists = [&](const std::string& candidate) {
                        for (Mesh* m : meshes) {
                            if (m && m->getName() == candidate) return true;
                        }
                        return false;
                    };
                    while (exists(name)) name = base + "_" + std::to_string(suffix++);
                    e.addMeshFromPrimitive(Cube{}, name);
                } catch (const std::exception& ex) {
                    Logger::log(std::string("[ConsoleMenu] Failed to add mesh: ") + ex.what());
                }
            });
        }},
        {"Custom mesh from file path", [this]() {
            awaitingLine = true;
            promptText = "Mesh path: ";
            lineHandler = [this](const std::string& rawPath) {
                const std::string path = rawPath;
                if (path.empty()) return;
                engine.enqueueMainThreadTask([path](Engine& e) {
                    try {
                        const std::string base = basenameOfPath(path);
                        std::string name = base;
                        int suffix = 1;
                        const std::vector<Mesh*> meshes = e.getMeshes();
                        auto exists = [&](const std::string& candidate) {
                            for (Mesh* m : meshes) {
                                if (m && m->getName() == candidate) return true;
                            }
                            return false;
                        };
                        while (exists(name)) name = base + "_" + std::to_string(suffix++);
                        e.addMeshFromObjFile(path, name);
                    } catch (const std::exception& ex) {
                        Logger::log(std::string("[ConsoleMenu] Failed to add mesh: ") + ex.what());
                    }
                });
            };
            dirty = true;
        }}
    };

    meshOptions->items = {
        {"Return", [this]() { pop(); }},
        {"Keys", [this]() {
            std::cout << "\n== Keys ==\n";
            std::cout << "Move: W/S (Z), A/D (X), Q/E (Y)\n";
            std::cout << "Rotate: Arrow keys, PageUp/PageDown\n";
            std::cout << "Scale: Z (up), X (down)\n";
            std::cout << "Reset: R\n";
            std::cout << "DrawMode: 1=Fill, 2=Line, 3=Point\n";
            std::cout << "RenderMode toggle: M\n";
            std::cout << "DebugLight toggle: L\n";
            std::cout << "Culling toggle: C\n";
            dirty = true;
        }},
        {"Change Texture", [this, snapshotTextureNames]() {
            if (editedMeshName.empty()) {
                std::cout << "No mesh selected.\n";
                dirty = true;
                return;
            }
            const std::vector<std::string> textures = snapshotTextureNames();
            if (textures.empty()) {
                std::cout << "No textures available.\n";
                dirty = true;
                return;
            }
            std::cout << "\nTextures List:\n";
            for (size_t i = 0; i < textures.size(); ++i)
                std::cout << "[" << i << "] " << textures[i] << "\n";

            awaitingLine = true;
            promptText = "Enter texture number: ";
            const std::string meshName = editedMeshName;
            lineHandler = [this, textures, meshName](const std::string& line) {
                size_t idx = 0;
                if (!parseIndex(line, idx) || idx >= textures.size()) {
                    std::cout << "Invalid texture number.\n";
                    dirty = true;
                    return;
                }
                const std::string textureName = textures[idx];
                engine.enqueueMainThreadTask([meshName, textureName](Engine& e) {
                    for (Mesh* m : e.getMeshes()) {
                        if (m && m->getName() == meshName) {
                            m->setTexture(textureName);
                            Logger::log(std::string("[Engine] Set texture for '") + meshName + "' => " + textureName);
                            return;
                        }
                    }
                    Logger::log(std::string("[Engine] Mesh not found: ") + meshName);
                });
            };
            dirty = true;
        }},
        {"Change Material", [this, snapshotMaterialNames]() {
            if (editedMeshName.empty()) {
                std::cout << "No mesh selected.\n";
                dirty = true;
                return;
            }
            const std::vector<std::string> materials = snapshotMaterialNames();
            if (materials.empty()) {
                std::cout << "No materials available.\n";
                dirty = true;
                return;
            }
            std::cout << "\nMaterials List:\n";
            for (size_t i = 0; i < materials.size(); ++i)
                std::cout << "[" << i << "] " << materials[i] << "\n";

            awaitingLine = true;
            promptText = "Enter material number: ";
            const std::string meshName = editedMeshName;
            lineHandler = [this, materials, meshName](const std::string& line) {
                size_t idx = 0;
                if (!parseIndex(line, idx) || idx >= materials.size()) {
                    std::cout << "Invalid material number.\n";
                    dirty = true;
                    return;
                }
                const std::string materialName = materials[idx];
                engine.enqueueMainThreadTask([meshName, materialName](Engine& e) {
                    for (Mesh* m : e.getMeshes()) {
                        if (m && m->getName() == meshName) {
                            m->setMaterial(materialName);
                            Logger::log(std::string("[Engine] Set material for '") + meshName + "' => " + materialName);
                            return;
                        }
                    }
                    Logger::log(std::string("[Engine] Mesh not found: ") + meshName);
                });
            };
            dirty = true;
        }},
    };
}

void ConsoleMenu::push(MenuNode* menu) {
    if (!menu) return;
    stack.push_back(menu);
    dirty = true;
}

void ConsoleMenu::pop() {
    if (stack.size() > 1) {
    stack.pop_back();
    dirty = true;
    }
}

void ConsoleMenu::renderCurrent() {
    if (stack.empty()) return;

    promptVisible = false;

    if (awaitingLine) {
        std::cout << "\n" << promptText;
        std::cout.flush();
        dirty = false;
        promptVisible = true;
        return;
    }

    MenuNode* cur = stack.back();
    std::cout << "\n== " << cur->title << " ==\n";
    for (size_t i = 0; i < cur->items.size(); ++i) {
        std::cout << "[" << i << "] " << cur->items[i].label << "\n";
    }
    std::cout << "> ";
    std::cout.flush();
    dirty = false;
    promptVisible = true;
}

std::string ConsoleMenu::trim(const std::string& s) {
    const char* ws = " \t\r\n";
    const size_t start = s.find_first_not_of(ws);
    if (start == std::string::npos) return "";
    const size_t end = s.find_last_not_of(ws);
    return s.substr(start, end - start + 1);
}

bool ConsoleMenu::parseIndex(const std::string& s, size_t& out) {
    try {
    size_t pos = 0;
    unsigned long v = std::stoul(s, &pos);
    if (pos != s.size()) return false;
    out = static_cast<size_t>(v);
    return true;
    } catch (...) {
    return false;
    }
}

void ConsoleMenu::handleLine(const std::string& line) {
    const std::string t = trim(line);

    if (awaitingLine) {
        if (t.empty() && !allowEmptyLineInPrompt) {
            awaitingLine = false;
            lineHandler = nullptr;
            dirty = true;
            return;
        }

        awaitingLine = false;
        allowEmptyLineInPrompt = false;

        auto handler = std::move(lineHandler);
        lineHandler = nullptr;
        if (handler) handler(t);
        dirty = true;
        return;
    }

    if (t.empty()) {
        dirty = true;
        return;
    }

    size_t idx = 0;
    if (!parseIndex(t, idx)) {
        std::cout << "Enter a number.\n";
        dirty = true;
        return;
    }

    if (stack.empty()) return;
    MenuNode* cur = stack.back();
    if (idx >= cur->items.size()) {
        std::cout << "Invalid choice.\n";
        dirty = true;
        return;
    }

    cur->items[idx].run();
}

void ConsoleMenu::threadMain() {
    const int fd = fileno(stdin);
    const int oldFlags = fcntl(fd, F_GETFL, 0);
    if (oldFlags != -1) fcntl(fd, F_SETFL, oldFlags | O_NONBLOCK);

    std::string buffer;
    std::vector<std::string> logs;

    while (running.load()) {
        logs.clear();
        Logger::drain(logs);
        if (!logs.empty()) {
            if (promptVisible) {
                // Clear current input line so logs don't appear after the prompt.
                std::cout << "\r\033[2K";
                promptVisible = false;
            }
            for (size_t i = 0; i < logs.size(); ++i) {
                std::cout << logs[i] << "\n";
            }
            std::cout.flush();
            dirty = true;
        }

    if (dirty.load()) renderCurrent();

    pollfd pfd;
    pfd.fd = fd;
    pfd.events = POLLIN;
    pfd.revents = 0;

    const int ret = poll(&pfd, 1, 100);
    if (ret <= 0) continue;

    if (pfd.revents & POLLIN) {
        char tmp[256];
        const ssize_t n = ::read(fd, tmp, sizeof(tmp));
        if (n > 0) {
          buffer.append(tmp, tmp + n);

          size_t pos = 0;
          while ((pos = buffer.find('\n')) != std::string::npos) {
              std::string line = buffer.substr(0, pos);
              buffer.erase(0, pos + 1);
              handleLine(line);
          }
        } else if (n == 0) {
            if (!isatty(fd)) {
                running = false;
            } else {
                dirty = true;
            }
        } else {
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                running = false;
            }
        }
    }
    }

    // Final drain: print anything enqueued right before shutdown.
    logs.clear();
    Logger::drain(logs);
    if (!logs.empty() && promptVisible) {
        std::cout << "\r\033[2K";
        promptVisible = false;
    }
    for (size_t i = 0; i < logs.size(); ++i) {
        std::cout << logs[i] << "\n";
    }
    if (!logs.empty()) std::cout.flush();

    if (oldFlags != -1) fcntl(fd, F_SETFL, oldFlags);
}
