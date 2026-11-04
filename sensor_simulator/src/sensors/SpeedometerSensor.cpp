#include "SensorData.hpp"
#include "sensors/SpeedometerSensor.hpp"

SensorData SpeedometerSensor::sample() {
    return {
        config_.vehicleId,
        config_.id,
        config_.type,
        generateValue(),
        currentTimeMs()
    };
}