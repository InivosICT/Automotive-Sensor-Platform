#include "SensorData.hpp"
#include "sensors/ClimateSensor.hpp"

SensorData ClimateSensor::sample() {
    return {
       config_.vehicleId,
       config_.id,
        config_.type,
        generateValue(),
        currentTimeMs()
    };
}