#include "UnixSocketClient.hpp"

#include <arpa/inet.h>
#include <cerrno>
#include <cstdint>
#include <cstring>
#include <limits>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <utility>

UnixSocketClient::UnixSocketClient(std::string socketPath) : m_socketPath(std::move(socketPath)){}

UnixSocketClient::~UnixSocketClient(){
    close();
}

bool UnixSocketClient::connect(){
    if(isConnected()){
        return true;
    }

    const auto now = std::chrono::steady_clock::now();

    // Do not attempt reconnection more than once every 2 seconds.
    if (now < m_nextReconnectAttempt) {
        return false;
    }

    m_nextReconnectAttempt = now + ReconnectInterval;


    sockaddr_un address{};
    address.sun_family = AF_UNIX;

    if (m_socketPath.empty() || m_socketPath.size() >= sizeof(address.sun_path))   {
        return false;
    }

    const int socketFd =
        ::socket(AF_UNIX, SOCK_STREAM, 0);

    if (socketFd == -1) {
        return false;
    }

    std::memcpy(
        address.sun_path,
        m_socketPath.c_str(),
        m_socketPath.size() + 1);

    const int result =
        ::connect(socketFd, reinterpret_cast<sockaddr*>(&address), sizeof(address));

    if (result == -1) {
        ::close(socketFd);
        return false;
    }
    m_socketFd = socketFd;
    return true;
}

bool UnixSocketClient::sendMessage(std::string_view payload) {

    if (payload.empty() || payload.size() > std::numeric_limits<std::uint32_t>::max()) {
        return false;
    }

     if (!isConnected() && !connect()) {
        return false;
    }

    const auto size = static_cast<std::uint32_t>(payload.size());

    const std::uint32_t networkSize = htonl(size);

    if (!sendAll(
            &networkSize,
            sizeof(networkSize))) {
        close();
        return false;
    }

    if (!sendAll(
            payload.data(),
            payload.size())) {
        close();
        return false;
    }

    return true;
}

bool UnixSocketClient::sendAll(const void* data, std::size_t size)
{
    const auto* bytes = static_cast<const unsigned char*>(data);

    std::size_t totalSent = 0;

    while (totalSent < size){
        const ssize_t sent=::send(m_socketFd, bytes + totalSent, size - totalSent, MSG_NOSIGNAL);
        
        if (sent > 0) {
            totalSent +=
                static_cast<std::size_t>(sent);

            continue;
        }

       if (sent == -1 &&
            errno == EINTR) {
            continue;
        }

        return false;
        }

     

    return true;
}

void UnixSocketClient::close() noexcept
{
    if (m_socketFd != -1)  {

        ::shutdown(
            m_socketFd,
            SHUT_RDWR
        );

        ::close(m_socketFd);
        m_socketFd = -1;
    }
}

bool UnixSocketClient::isConnected() const noexcept
{
   return m_socketFd != -1;
}