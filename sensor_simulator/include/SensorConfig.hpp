#pragma once

#include <string>

struct SensorConfig {
    std::string vehicleId;
    std::string id;
    std::string type;
    int periodSec{};
    double minValue{};
    double maxValue{};
};