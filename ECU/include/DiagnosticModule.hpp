#pragma once

#include "ActuatorTypes.hpp"
#include "DiagnosticTypes.hpp"
#include "SensorData.hpp"

#include <cstdint>
#include <vector>

struct DiagnosticConfig
{
    double maximumRpm{6500.0};
    double maximumSpeedKph{120.0};

    double fanActivationTemperature{105.0};
    double fanDeactivationTemperature{98.0};

    std::uint64_t staleTimeoutMs{5000};
};

struct DiagnosticResult
{
    std::vector<DiagnosticEvent> events;
    std::vector<ActuatorCommand> commands;
};

class DiagnosticModule
{
public:
    explicit DiagnosticModule(
        DiagnosticConfig config);

    DiagnosticResult evaluate(
        const std::vector<SensorData>& state,
        std::uint64_t currentTimestampMs);

private:
    DiagnosticConfig m_config;

    bool m_coolingFanActive{false};
    bool m_engineLimiterActive{false};
    bool m_overspeedWarningActive{false};
};