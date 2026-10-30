#pragma once

#include "ISensor.hpp"
class RpmSensor : public ISensor {
    public:
        SensorData sample() const override;
};