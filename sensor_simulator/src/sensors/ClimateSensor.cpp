#include "sensor_simulator/include/SensorData.hpp";
#include "sensor_simulator/include/sensors/ClimateSensor.hpp";

SensorData ClimateSensor::sample() const {
    return {
        "001",
        "temperature",
        50.02,
        12500
    };
}