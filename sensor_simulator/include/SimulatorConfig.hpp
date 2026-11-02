#pragma once

#include "SensorConfig.hpp"

#include <string>
#include <vector>

struct SimulatorConfig
{
    std::string vehicleId;
    std::string ecuSocketPath;

    std::vector<SensorConfig> sensors;
};