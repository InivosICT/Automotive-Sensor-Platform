#include "SensorData.hpp"
#include "sensors/ClimateSensor.hpp"

SensorData ClimateSensor::sample() const {
    return {
        "001",
        "temperature",
        50.02,
        12500
    };
}