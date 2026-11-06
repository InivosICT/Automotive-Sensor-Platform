#pragma once

#include <string>

enum class DiagnosticSeverity
{
    Information,
    Warning,
    Critical
};

enum class DiagnosticCode
{
    HighRpm,
    Overspeed,
    HighTemperature,
    InvalidValue,
    StaleSensor
};

struct DiagnosticEvent
{
    DiagnosticCode code;
    DiagnosticSeverity severity;

    std::string sensorId;
    std::string message;

    double observedValue{};
};