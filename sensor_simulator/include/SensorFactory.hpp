#pragma once

#include <memory>
#include "ISensor.hpp"
#include "SensorConfig.hpp"

class SensorFactory {
    public:
        static std::unique_ptr<ISensor> create(const SensorConfig&);
};