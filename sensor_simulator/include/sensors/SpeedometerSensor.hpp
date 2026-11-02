#pragma once

#include "ISensor.hpp"
class SpeedometerSensor : public ISensor {
    public:
        SensorData sample() const override;
};