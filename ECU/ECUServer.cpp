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

ECUServer::ECUServer(
    std::string socketPath)
    : m_socketPath(std::move(socketPath))
{
}

ECUServer::~ECUServer()
{
    stop();
}

bool ECUServer::start()
{
    stop();

    m_serverFd =
        ::socket(AF_UNIX, SOCK_STREAM, 0);

    if (m_serverFd == -1)
    {
        return false;
    }

    ::unlink(m_socketPath.c_str());

    sockaddr_un address{};
    address.sun_family = AF_UNIX;

    std::strncpy(
        address.sun_path,
        m_socketPath.c_str(),
        sizeof(address.sun_path) - 1);

    if (::bind(
            m_serverFd,
            reinterpret_cast<sockaddr*>(&address),
            sizeof(address)) == -1)
    {
        stop();
        return false;
    }

    if (::listen(
            m_serverFd,
            10) == -1)
    {
        stop();
        return false;
    }

    return true;
}

void ECUServer::run()
{
    while (true)
    {
        const int clientFd =
            ::accept(
                m_serverFd,
                nullptr,
                nullptr);

        if (clientFd == -1)
        {
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
    auto* bytes =
        static_cast<unsigned char*>(buffer);

    std::size_t totalReceived = 0;

    while (totalReceived < size)
    {
        const ssize_t received =
            ::recv(
                fd,
                bytes + totalReceived,
                size - totalReceived,
                0);

        if (received <= 0)
        {
            return false;
        }

        totalReceived +=
            static_cast<std::size_t>(
                received);
    }

    return true;
}

void ECUServer::handleClient(
    int clientFd)
{
    while (true)
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

        if (document.HasParseError())
        {
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
    }

    ::close(clientFd);
}

void ECUServer::stop() noexcept
{
    if (m_serverFd != -1)
    {
        ::close(m_serverFd);

        m_serverFd = -1;
    }

    ::unlink(
        m_socketPath.c_str());
}