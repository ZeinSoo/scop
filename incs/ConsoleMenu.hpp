#pragma once

#include <atomic>
#include <functional>
#include <memory>
#include <string>
#include <thread>
#include <vector>

class Engine;

class ConsoleMenu {
    public:
        explicit ConsoleMenu(Engine& engine);
        ~ConsoleMenu();

        void start();
        void stop();

    private:
        struct Entry {
            std::string label;
            std::function<void()> run;
        };

        struct MenuNode {
            std::string title;
            std::vector<Entry> items;
        };

        Engine& engine;
        std::atomic<bool> running;
        std::atomic<bool> dirty;
        std::thread worker;

        std::unique_ptr<MenuNode> home;
        std::unique_ptr<MenuNode> textureManager;
        std::unique_ptr<MenuNode> lightManager;
        std::unique_ptr<MenuNode> meshManager;
        std::unique_ptr<MenuNode> addMesh;
        std::unique_ptr<MenuNode> meshOptions;
        std::vector<MenuNode*> stack;

        std::string editedMeshName;

        bool promptVisible;

        bool awaitingLine;
        bool allowEmptyLineInPrompt;
        std::string promptText;
        std::function<void(const std::string&)> lineHandler;

        void buildMenus();
        void threadMain();

        void renderCurrent();
        void push(MenuNode* menu);
        void pop();

        void handleLine(const std::string& line);

        static std::string trim(const std::string& s);
        static bool parseIndex(const std::string& s, size_t& out);
};
