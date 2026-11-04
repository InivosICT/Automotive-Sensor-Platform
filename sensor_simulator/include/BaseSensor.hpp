#pragma once

#include <cstdint>
#include <random>

#include "ISensor.hpp"
#include "SensorConfig.hpp"

class BaseSensor : public ISensor {
public:
    explicit BaseSensor(const SensorConfig& config);

protected:
    double generateValue();
    static std::uint64_t currentTimeMs();

    SensorConfig config_;

private:
    std::mt19937 randomEngine_;
    std::uniform_real_distribution<double> distribution_;
};