#include "SingleInstanceGuard.hpp"

#include <cerrno>
#include <cstring>
#include <stdexcept>
#include <string>

#include <fcntl.h>
#include <sys/file.h>
#include <unistd.h>

SingleInstanceGuard::SingleInstanceGuard(
    const std::string& lockFilePath)
{
    m_lockFd = ::open(
        lockFilePath.c_str(),
        O_CREAT | O_RDWR,
        0644
    );

    if (m_lockFd == -1) {
        throw std::runtime_error(
            "Failed to open ECU lock file '" +
            lockFilePath +
            "': " +
            std::strerror(errno)
        );
    }

    if (::flock(
            m_lockFd,
            LOCK_EX | LOCK_NB) == -1) {

        const int errorCode = errno;

        ::close(m_lockFd);
        m_lockFd = -1;

        if (errorCode == EWOULDBLOCK ||
            errorCode == EAGAIN) {

            throw std::runtime_error(
                "Another ECU processor is already running"
            );
        }

        throw std::runtime_error(
            "Failed to lock ECU lock file: " +
            std::string{std::strerror(errorCode)}
        );
    }

    // Save this process's PID in the lock file.
    const std::string processId =
        std::to_string(::getpid()) + '\n';

    if (::ftruncate(m_lockFd, 0) == -1) {
        const int errorCode = errno;

        ::flock(m_lockFd, LOCK_UN);
        ::close(m_lockFd);
        m_lockFd = -1;

        throw std::runtime_error(
            "Failed to clear ECU lock file: " +
            std::string{std::strerror(errorCode)}
        );
    }

    const ssize_t written = ::write(
        m_lockFd,
        processId.data(),
        processId.size()
    );

    if (written !=
        static_cast<ssize_t>(processId.size())) {

        const int errorCode = errno;

        ::flock(m_lockFd, LOCK_UN);
        ::close(m_lockFd);
        m_lockFd = -1;

        throw std::runtime_error(
            "Failed to write ECU PID: " +
            std::string{std::strerror(errorCode)}
        );
    }
}

SingleInstanceGuard::~SingleInstanceGuard()
{
    if (m_lockFd != -1) {
        ::flock(m_lockFd, LOCK_UN);
        ::close(m_lockFd);
        m_lockFd = -1;
    }
}