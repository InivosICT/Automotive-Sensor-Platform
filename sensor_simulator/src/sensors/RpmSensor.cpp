#include "SensorData.hpp"
#include "sensors/RpmSensor.hpp"

SensorData RpmSensor::sample() {
    return {
        config_.vehicleId,
        config_.id,
        config_.type,
        generateValue(),
        currentTimeMs()
    };
}