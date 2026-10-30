#include "sensor_simulator/include/SensorData.hpp";
#include "sensor_simulator/include/sensors/RpmSensor.hpp";

SensorData RpmSensor::sample() const {
    return {
        "001",
        "rpm",
        50.02,
        12500
    };
}