#pragma once

#include <string>

class SingleInstanceGuard
{
public:
    explicit SingleInstanceGuard(
        const std::string& lockFilePath);

    ~SingleInstanceGuard();

    SingleInstanceGuard(
        const SingleInstanceGuard&) = delete;

    SingleInstanceGuard& operator=(
        const SingleInstanceGuard&) = delete;

    SingleInstanceGuard(
        SingleInstanceGuard&&) = delete;

    SingleInstanceGuard& operator=(
        SingleInstanceGuard&&) = delete;

private:
    int m_lockFd{-1};
};