#include "BaseSensor.hpp"

#include <chrono>
#include <stdexcept>
#include <iostream>

BaseSensor::BaseSensor(const SensorConfig& config)
    : config_(config),
      randomEngine_(std::random_device{}()),
      distribution_(config.minValue, config.maxValue)
{
     std::cout << "Creating sensor:\n"
              << "  id       = " << config_.id << '\n'
              << "  type     = " << config_.type << '\n'
              << "  minValue = " << config_.minValue << '\n'
              << "  maxValue = " << config_.maxValue << '\n';

    if (config.minValue > config.maxValue) {
        throw std::invalid_argument(
            "Sensor minValue cannot be greater than maxValue");
    }
}

double BaseSensor::generateValue()
{
    return distribution_(randomEngine_);
}

std::uint64_t BaseSensor::currentTimeMs()
{
    const auto now = std::chrono::system_clock::now();

    const auto milliseconds =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch());

    return static_cast<std::uint64_t>(milliseconds.count());
}