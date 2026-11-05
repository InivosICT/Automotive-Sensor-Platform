#pragma once

#include <fstream>
#include <mutex>
#include <string>

class Logger
{
public:
    static Logger& instance();

    void initialize(const std::string& filePath);

    void info(const std::string& message);
    void warning(const std::string& message);
    void error(const std::string& message);

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

private:
    Logger() = default;
    ~Logger() = default;

    void log(
        const std::string& level,
        const std::string& message);

    std::ofstream m_logFile;
    std::mutex m_mutex;
};