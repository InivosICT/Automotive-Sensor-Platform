#pragma once

#include <string>

class ECUServer
{
public:
    explicit ECUServer(std::string socketPath);

    ~ECUServer();

    ECUServer(const ECUServer&) = delete;
    ECUServer& operator=(const ECUServer&) = delete;

    bool start();
    void run();
    void stop() noexcept;

private:
    void handleClient(int clientFd);

    bool recvAll(int fd, void* buffer, std::size_t size);

    std::string m_socketPath;

    int m_serverFd{-1};
};