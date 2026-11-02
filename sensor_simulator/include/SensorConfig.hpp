#pragma once

#include <string>

struct SensorConfig {
    std::string id;
    std::string type;
    int periodSec;
};