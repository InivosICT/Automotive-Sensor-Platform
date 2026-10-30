#pragma once

#include "ISensor.hpp"
class ClimateSensor : public ISensor {
    public:
        SensorData sample() const override;

};