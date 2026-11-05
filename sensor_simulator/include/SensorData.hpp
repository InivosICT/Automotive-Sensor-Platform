#pragma once

#include <string>
#include <cstdint>

struct SensorData
{   
    std::string vehicleId;
    std::string sensorId;
    std::string sensorType;
    double value{};
    std::uint64_t timestampMs{};
};
