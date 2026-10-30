#pragma once

#include "SensorData.hpp"

class ISensor {
    public:
        
        virtual ~ISensor() = default;
        virtual SensorData sample() const = 0;
};