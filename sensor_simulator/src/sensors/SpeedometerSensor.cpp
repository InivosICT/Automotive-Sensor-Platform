#include "sensor_simulator/include/SensorData.hpp";
#include "sensor_simulator/include/sensors/SpeedometerSensor.hpp";

SensorData SpeedometerSensor::sample() const {
    return {
        "001",
        "speedometer",
        50.02,
        12500
    };
}