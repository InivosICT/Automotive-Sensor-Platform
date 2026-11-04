#pragma once

#include "SensorData.hpp"

#include <string>

class PayloadBuilder
{
public:
    std::string build(const SensorData& data) const;
};