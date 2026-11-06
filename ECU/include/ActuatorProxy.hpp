#pragma once

#include "ActuatorTypes.hpp"

#include <mutex>
#include <unordered_map>

class ActuatorProxy
{
public:
    void execute(
        const ActuatorCommand& command);

private:
    struct ActuatorTypeHash
    {
        std::size_t operator()(
            const ActuatorType actuator) const noexcept
        {
            return static_cast<std::size_t>(
                actuator
            );
        }
    };

    std::mutex m_mutex;

    std::unordered_map<
        ActuatorType,
        ActuatorCommand,
        ActuatorTypeHash> m_lastCommands;
};