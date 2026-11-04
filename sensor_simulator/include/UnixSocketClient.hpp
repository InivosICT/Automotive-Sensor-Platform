#pragma once

#include <cstddef>
#include <string>
#include <string_view>

class UnixSocketClient
{
public:
    explicit UnixSocketClient(std::string socketPath);

    ~UnixSocketClient();
    UnixSocketClient(const UnixSocketClient&) = delete;

    UnixSocketClient& operator=(const UnixSocketClient&) = delete;

    bool connect();
    bool sendMessage(std::string_view payload);
    void close() noexcept;

    bool isConnected() const noexcept;

private:
    bool sendAll(const void* data,std::size_t size);

    std::string m_socketPath;
    int m_socketFd{-1};
};