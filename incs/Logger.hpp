#pragma once

#include <string>
#include <vector>

class Logger {
    public:
        // Thread safe: push a log line.
        static void log(const std::string& message);

        // Thread safe: drain all pending logs into out.
        static void drain(std::vector<std::string>& out);
    private:
        Logger();
};