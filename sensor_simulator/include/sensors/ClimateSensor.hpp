#pragma once

#include "BaseSensor.hpp"

class ClimateSensor : public BaseSensor {
    public:
        using BaseSensor::BaseSensor;
        SensorData sample() override;

};