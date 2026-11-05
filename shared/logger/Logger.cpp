#include "Logger.hpp"

#include <chrono>
#include <iomanip>
#include <iostream>

Logger& Logger::instance(){
    static Logger logger;
    return logger;
}

void Logger::initialize(const std::string& filePath){
    std::lock_guard<std::mutex> lock(m_mutex);

    if (!m_logFile.is_open()){
        m_logFile.open(filePath,std::ios::app);
    }
}

void Logger::info(const std::string& message)
{
    log("INFO", message);
}

void Logger::warning(const std::string& message)
{
    log("WARNING", message);
}

void Logger::error(const std::string& message)
{
    log("ERROR", message);
}

void Logger::log(const std::string& level, const std::string& message)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    const auto now = std::chrono::system_clock::now();

    const auto currentTime = std::chrono::system_clock::to_time_t(now);

    m_logFile
        << std::put_time(
               std::localtime(
                   &currentTime),
               "%Y-%m-%d %H:%M:%S")
        << " ["
        << level
        << "] "
        << message
        << '\n';

    m_logFile.flush();
}