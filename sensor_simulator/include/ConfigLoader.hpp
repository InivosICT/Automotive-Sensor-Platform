#pragma once

#include "SimulatorConfig.hpp"

#include <filesystem>

class ConfigLoader
{
public:
    static SimulatorConfig load(
        const std::filesystem::path& configPath);
};