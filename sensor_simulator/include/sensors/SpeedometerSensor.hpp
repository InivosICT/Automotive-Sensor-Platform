#pragma once

#include "BaseSensor.hpp"

class SpeedometerSensor : public BaseSensor {
    public:
        using BaseSensor::BaseSensor;
        SensorData sample() override;
};