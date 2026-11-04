#pragma once

#include "BaseSensor.hpp"

class RpmSensor : public BaseSensor {
    public:
    using BaseSensor::BaseSensor;
        SensorData sample()  override;
};