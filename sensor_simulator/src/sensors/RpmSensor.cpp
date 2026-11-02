#include "SensorData.hpp"
#include "sensors/RpmSensor.hpp"

SensorData RpmSensor::sample() const {
    return {
        "001",
        "rpm",
        50.02,
        12500
    };
}