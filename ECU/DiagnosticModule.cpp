#include "DiagnosticModule.hpp"

#include <cmath>
#include <utility>

DiagnosticModule::DiagnosticModule(DiagnosticConfig config) : m_config(std::move(config)) {
}

DiagnosticResult DiagnosticModule::evaluate(const std::vector<SensorData>& state, const std::uint64_t currentTimestampMs) {
    DiagnosticResult result;

    for (const SensorData& sensor : state) {
        if (!std::isfinite(sensor.value)) {
            result.events.push_back({
                DiagnosticCode::InvalidValue,
                DiagnosticSeverity::Critical,
                sensor.sensorId,
                "Sensor produced a non-finite value",
                sensor.value
            });

            result.commands.push_back({
                ActuatorType::DegradedMode,
                ActuatorAction::Activate,
                1.0,
                "Invalid sensor value from " +
                    sensor.sensorId
            });

            continue;
        }

        const bool timestampInFuture = sensor.timestampMs > currentTimestampMs;

        const bool stale = !timestampInFuture && currentTimestampMs - sensor.timestampMs > m_config.staleTimeoutMs;

        if (timestampInFuture || stale) {
            result.events.push_back({
                DiagnosticCode::StaleSensor,
                DiagnosticSeverity::Warning,
                sensor.sensorId,
                timestampInFuture
                    ? "Sensor timestamp is in the future"
                    : "Sensor value is stale",
                sensor.value
            });

            result.commands.push_back({
                ActuatorType::DashboardWarning,
                ActuatorAction::Activate,
                1.0,
                "Unreliable sensor: " +
                    sensor.sensorId
            });

            continue;
        }

        if (sensor.sensorType == "RPM") {
            if (sensor.value >
                    m_config.maximumRpm &&
                !m_engineLimiterActive) {

                result.events.push_back({
                    DiagnosticCode::HighRpm,
                    DiagnosticSeverity::Critical,
                    sensor.sensorId,
                    "RPM exceeds configured maximum",
                    sensor.value
                });

                result.commands.push_back({
                    ActuatorType::EngineLimiter,
                    ActuatorAction::Activate,
                    m_config.maximumRpm,
                    "High RPM"
                });

                m_engineLimiterActive = true;
            }
            else if (
                sensor.value <
                    m_config.maximumRpm - 500.0 &&
                m_engineLimiterActive) {

                result.commands.push_back({
                    ActuatorType::EngineLimiter,
                    ActuatorAction::Deactivate,
                    0.0,
                    "RPM returned to safe range"
                });

                m_engineLimiterActive = false;
            }
        }
        else if (
            sensor.sensorType == "Speedometer") {

            if (sensor.value >
                    m_config.maximumSpeedKph &&
                !m_overspeedWarningActive) {

                result.events.push_back({
                    DiagnosticCode::Overspeed,
                    DiagnosticSeverity::Warning,
                    sensor.sensorId,
                    "Vehicle speed exceeds limit",
                    sensor.value
                });

                result.commands.push_back({
                    ActuatorType::DashboardWarning,
                    ActuatorAction::Activate,
                    sensor.value,
                    "Overspeed"
                });

                m_overspeedWarningActive = true;
            }
            else if (
                sensor.value <
                    m_config.maximumSpeedKph - 5.0 &&
                m_overspeedWarningActive) {

                result.commands.push_back({
                    ActuatorType::DashboardWarning,
                    ActuatorAction::Deactivate,
                    0.0,
                    "Vehicle speed returned to safe range"
                });

                m_overspeedWarningActive = false;
            }
        }
        else if (
            sensor.sensorType == "Climate") {

            if (sensor.value >=
                    m_config
                        .fanActivationTemperature &&
                !m_coolingFanActive) {

                result.events.push_back({
                    DiagnosticCode::HighTemperature,
                    DiagnosticSeverity::Critical,
                    sensor.sensorId,
                    "Temperature exceeds cooling threshold",
                    sensor.value
                });

                result.commands.push_back({
                    ActuatorType::CoolingFan,
                    ActuatorAction::SetLevel,
                    100.0,
                    "High temperature"
                });

                m_coolingFanActive = true;
            }
            else if (
                sensor.value <=
                    m_config
                        .fanDeactivationTemperature &&
                m_coolingFanActive) {

                result.commands.push_back({
                    ActuatorType::CoolingFan,
                    ActuatorAction::Deactivate,
                    0.0,
                    "Temperature returned to safe range"
                });

                m_coolingFanActive = false;
            }
        }
    }

    return result;
}