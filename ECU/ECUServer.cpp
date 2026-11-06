#include "ECUServer.hpp"
#include "Logger.hpp"

#include <rapidjson/document.h>

#include <arpa/inet.h>
#include <cstdint>
#include <cstring>
#include <iostream>

#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <thread>

namespace
{
constexpr std::size_t MaximumPayloadSize =
    1024 * 1024; // 1 MiB

bool hasValidPayloadFields(
    const rapidjson::Document& document)
{
    return document.IsObject() &&
           document.HasMember("vehicleId") &&
           document["vehicleId"].IsString() &&
           document.HasMember("sensorId") &&
           document["sensorId"].IsString() &&
           document.HasMember("sensorType") &&
           document["sensorType"].IsString() &&
           document.HasMember("value") &&
           document["value"].IsNumber() &&
           document.HasMember("timestampMs") &&
           document["timestampMs"].IsUint64();
}
std::string severityToString(
    const DiagnosticSeverity severity)
{
    switch (severity) {
    case DiagnosticSeverity::Information:
        return "Information";

    case DiagnosticSeverity::Warning:
        return "Warning";

    case DiagnosticSeverity::Critical:
        return "Critical";
    }

    return "Unknown";
}
}

ECUServer& ECUServer::instance()
{
    static ECUServer server;
    return server;
}

// ECUServer::ECUServer(
//     std::string socketPath)
//     : m_socketPath(std::move(socketPath))
// {
// }

ECUServer::ECUServer()
    : m_diagnostics(
          DiagnosticConfig{
              6500.0, // Maximum RPM
              120.0,  // Maximum speed
              105.0,  // Fan activation temperature
              98.0,   // Fan deactivation temperature
              5000    // Stale timeout in milliseconds
          })
{
}


ECUServer::~ECUServer()
{
    stop();
}

bool ECUServer::start(const std::string& socketPath){

     if (m_running.load()) {
        Logger::instance().warning(
            "ECU server is already running"
        );

        return false;
    }

    if (socketPath.empty()) {
        Logger::instance().error(
            "ECU socket path cannot be empty"
        );

        return false;
    }

    sockaddr_un address{};

    if (socketPath.size() >= sizeof(address.sun_path)) {

        Logger::instance().error( "ECU socket path is too long: " + socketPath   );

        return false;
    }

    stop();

    m_socketPath = socketPath;

    m_serverFd =
        ::socket(AF_UNIX, SOCK_STREAM, 0);

    if (m_serverFd == -1) {
        Logger::instance().error( "Failed to create ECU socket: " + std::string{std::strerror(errno)}  );

        m_socketPath.clear();
        return false;
    }

    address.sun_family = AF_UNIX;

    std::strncpy(
        address.sun_path,
        m_socketPath.c_str(),
        sizeof(address.sun_path) - 1);
    
    ::unlink(m_socketPath.c_str());

    if (::bind( 
            m_serverFd,
            reinterpret_cast<const sockaddr*>(&address),
            sizeof(address)) == -1){
        
        Logger::instance().error(
            "Failed to bind ECU socket: " +
            std::string{std::strerror(errno)}
        );

        stop();
        return false;
    }

    constexpr int connectionBacklog = 10;

    if (::listen(m_serverFd, connectionBacklog) == -1){
        stop();
        return false;
    }

    m_running.store(true);

    Logger::instance().info( "ECU server started, listening on: " + m_socketPath );
    return true;
}

void ECUServer::run()
{   
    if (!m_running.load() ||
        m_serverFd == -1) {

        Logger::instance().error(
            "ECU server must be started before run()"
        );

        return;
    }

    while (m_running.load())
    {
        const int clientFd =
            ::accept(
                m_serverFd,
                nullptr,
                nullptr);

        if (clientFd == -1)  {
             if (errno == EINTR) {
                continue;
            }

            if (!m_running.load()) {
                break;
            }

            Logger::instance().error("Failed to accept client: " + std::string{std::strerror(errno)}
            );

            continue;
        }

        std::thread(
            &ECUServer::handleClient,
            this,
            clientFd)
            .detach();
    }
}

bool ECUServer::recvAll(
    int fd,
    void* buffer,
    std::size_t size)
{
    auto* bytes = static_cast<unsigned char*>(buffer);

    std::size_t totalReceived = 0;

    while (totalReceived < size)
    {
        const ssize_t received =
            ::recv(
                fd,
                bytes + totalReceived,
                size - totalReceived,
                0);

         if (received == 0) {
            // Client disconnected normally.
            return false;
        }

        if (received < 0) {
            if (errno == EINTR) {
                continue;
            }
            Logger::instance().error(
                "Socket receive failed: " +
                std::string{std::strerror(errno)}
            );
            return false;
        }

        totalReceived +=
            static_cast<std::size_t>(
                received);
    }

    return true;
}

void ECUServer::handleClient(
    const int clientFd)
{
    Logger::instance().info("Sensor simulator client connected");
    while (m_running.load())
    {
        std::uint32_t networkSize{};

        if (!recvAll(
                clientFd,
                &networkSize,
                sizeof(networkSize)))
        {
            break;
        }

        const auto payloadSize =
            ntohl(networkSize);

        if (payloadSize == 0 ||
            payloadSize > MaximumPayloadSize) {

            Logger::instance().error(
                "Rejected invalid payload size: " +
                std::to_string(payloadSize)
            );

            break;
        }
        
        std::string payload(
            payloadSize,
            '\0');

        if (!recvAll(
                clientFd,
                payload.data(),
                payloadSize))
        {
            break;
        }

        rapidjson::Document document;

        document.Parse(
            payload.c_str());

        if (document.HasParseError()){
            Logger::instance().warning( "Rejected malformed JSON payload"  );
            continue;
        }

        if (!hasValidPayloadFields(document)) {
            Logger::instance().warning(
                "Rejected payload with missing or "
                "invalid fields"
            );

            continue;
        }

        const std::string sensorId =
            document["sensorId"].GetString();

        const std::string sensorType =
            document["sensorType"].GetString();

        const double value =
            document["value"].GetDouble();

        const std::uint64_t timestamp =
            document["timestampMs"].GetUint64();

        std::cout
            << "Vehicle ID: "
            << document["vehicleId"].GetString()
            << ", Sensor ID: "
            << sensorId
            << ", Type: "
            << sensorType
            << ", Value: "
            << value
            << ", Timestamp: "
            << timestamp
            << '\n';
        Logger::instance().info(
            "Vehicle ID: " + std::string(document["vehicleId"].GetString()) +
            ", Sensor ID: " + sensorId +
            ", Type: " + sensorType +
            ", Value: " + std::to_string(value) +
            ", Timestamp: " + std::to_string(timestamp));
        
            const SensorData sensorData{
    document["vehicleId"].GetString(),
    document["sensorId"].GetString(),
    document["sensorType"].GetString(),
    document["value"].GetDouble(),
    document["timestampMs"].GetUint64()
        };  
    process(sensorData);   
}
    
    ::shutdown(clientFd, SHUT_RDWR);

    ::close(clientFd);
    Logger::instance().info(
        "Sensor simulator client disconnected"
    );
}

void ECUServer::stop() noexcept{
    m_running.store(false);
    if (m_serverFd != -1)
    {
        ::shutdown(
            m_serverFd,
            SHUT_RDWR
        );
        ::close(m_serverFd);

        m_serverFd = -1;
    }

   if (!m_socketPath.empty()) {
        ::unlink(m_socketPath.c_str());
        m_socketPath.clear();
    }
}

std::uint64_t ECUServer::currentTimestampMs()
{
    return static_cast<std::uint64_t>(
        std::chrono::duration_cast<
            std::chrono::milliseconds>(
            std::chrono::system_clock::now()
                .time_since_epoch()
        ).count()
    );
}

void ECUServer::process(
    const SensorData& sensorData)
{
    std::lock_guard<std::mutex> lock{
        m_processMutex
    };

    m_state.update(sensorData);

    Logger::instance().info(
        "ECU state updated: vehicle=" +
        sensorData.vehicleId +
        ", sensor=" +
        sensorData.sensorId +
        ", type=" +
        sensorData.sensorType +
        ", value=" +
        std::to_string(sensorData.value)
    );

    const DiagnosticResult result =
        m_diagnostics.evaluate(
            m_state.snapshot(),
            currentTimestampMs()
        );

    for (const DiagnosticEvent& event :
         result.events) {

        const std::string message =
            "Diagnostic: severity=" +
            severityToString(event.severity) +
            ", sensor=" +
            event.sensorId +
            ", message=" +
            event.message +
            ", value=" +
            std::to_string(
                event.observedValue);

        if (event.severity ==
            DiagnosticSeverity::Critical) {
            Logger::instance().error(message);
        }
        else if (
            event.severity ==
            DiagnosticSeverity::Warning) {
            Logger::instance().warning(message);
        }
        else {
            Logger::instance().info(message);
        }
    }

    for (const ActuatorCommand& command :
         result.commands) {
        m_actuatorProxy.execute(command);
    }
}

void ECUServer::runPeriodicDiagnostics()
{
    std::lock_guard<std::mutex> lock{
        m_processMutex
    };

    const DiagnosticResult result =
        m_diagnostics.evaluate(
            m_state.snapshot(),
            currentTimestampMs()
        );

    for (const ActuatorCommand& command :
         result.commands) {
        m_actuatorProxy.execute(command);
    }
}

