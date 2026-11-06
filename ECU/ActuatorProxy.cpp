#include "ActuatorProxy.hpp"
#include "Logger.hpp"

#include <string>

namespace
{
std::string toString(
    const ActuatorType actuator)
{
    switch (actuator) {
    case ActuatorType::CoolingFan:
        return "CoolingFan";

    case ActuatorType::EngineLimiter:
        return "EngineLimiter";

    case ActuatorType::DashboardWarning:
        return "DashboardWarning";

    case ActuatorType::DegradedMode:
        return "DegradedMode";
    }

    return "UnknownActuator";
}

std::string toString(
    const ActuatorAction action)
{
    switch (action) {
    case ActuatorAction::Activate:
        return "Activate";

    case ActuatorAction::Deactivate:
        return "Deactivate";

    case ActuatorAction::SetLevel:
        return "SetLevel";
    }

    return "UnknownAction";
}
}

void ActuatorProxy::execute(
    const ActuatorCommand& command)
{
    std::lock_guard<std::mutex> lock{
        m_mutex
    };

    const auto iterator =
        m_lastCommands.find(command.actuator);

    if (iterator != m_lastCommands.end()) {
        const ActuatorCommand& previous =
            iterator->second;

        if (previous.action == command.action &&
            previous.requestedValue ==
                command.requestedValue) {
            return;
        }
    }

    m_lastCommands[command.actuator] =
        command;

    Logger::instance().warning(
        "Actuator command: actuator=" +
        toString(command.actuator) +
        ", action=" +
        toString(command.action) +
        ", requestedValue=" +
        std::to_string(
            command.requestedValue) +
        ", reason=" +
        command.reason
    );

    /*
     * Current implementation:
     *     simulated actuation through logging.
     *
     * Future implementation:
     *     send command through another Unix socket,
     *     CAN interface, SOME/IP service, or HAL.
     */
}