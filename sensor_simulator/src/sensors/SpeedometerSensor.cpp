#include "SensorData.hpp"
#include "sensors/SpeedometerSensor.hpp"

SensorData SpeedometerSensor::sample() const {
    return {
        "001",
        "speedometer",
        50.02,
        12500
    };
}