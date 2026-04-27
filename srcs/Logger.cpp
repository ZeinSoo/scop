#include "Logger.hpp"

#include <mutex>
#include <queue>

static std::mutex g_loggerMutex;
static std::queue<std::string> g_loggerQueue;

void Logger::log(const std::string& message) {
    std::lock_guard<std::mutex> lock(g_loggerMutex);
    g_loggerQueue.push(message);
}

void Logger::drain(std::vector<std::string>& out) {
    std::queue<std::string> local;
    {
        std::lock_guard<std::mutex> lock(g_loggerMutex);
        local.swap(g_loggerQueue);
    }

    while (!local.empty()) {
        out.push_back(local.front());
        local.pop();
    }
}

Logger::Logger() {}