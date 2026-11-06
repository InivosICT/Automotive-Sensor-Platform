#pragma once

#include <string>

enum class ActuatorType
{
    CoolingFan,
    EngineLimiter,
    DashboardWarning,
    DegradedMode
};

enum class ActuatorAction
{
    Activate,
    Deactivate,
    SetLevel
};

struct ActuatorCommand
{
    ActuatorType actuator;
    ActuatorAction action;

    double requestedValue{};
    std::string reason;
};