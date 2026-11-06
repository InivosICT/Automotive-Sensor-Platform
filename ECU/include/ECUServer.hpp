#pragma once

#include <string>
#include <atomic>

#include "SensorData.hpp"
#include "InternalStateMap.hpp"
#include "DiagnosticModule.hpp"
#include "ActuatorProxy.hpp"

class ECUServer
{
public:
    
    static ECUServer& instance();
    // explicit ECUServer(std::string socketPath);

    ~ECUServer();

    ECUServer(const ECUServer&) = delete;
    ECUServer& operator=(const ECUServer&) = delete;

    ECUServer(ECUServer&&) = delete;
    ECUServer& operator=(ECUServer&&) = delete;

    bool start(const std::string& socketPath);
    void run();
    void stop() noexcept;


    void runPeriodicDiagnostics();



private:
    ECUServer();
    
    void handleClient(int clientFd);

    bool recvAll(int fd, void* buffer, std::size_t size);

    std::string m_socketPath;

    int m_serverFd{-1};

    std::atomic_bool m_running{false};

    static std::uint64_t currentTimestampMs();
    void process(const SensorData& sensorData);

    std::mutex m_processMutex;

    InternalStateMap m_state;
    DiagnosticModule m_diagnostics;
    ActuatorProxy m_actuatorProxy;

};