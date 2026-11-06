#pragma once

#include "SensorData.hpp"

#include <optional>
#include <shared_mutex>
#include <string>
#include <unordered_map>
#include <vector>

class InternalStateMap
{
public:
    void update(const SensorData& sensorData);

    std::optional<SensorData> getById(const std::string& sensorId) const;

    std::vector<SensorData> snapshot() const;

private:
    mutable std::shared_mutex m_mutex;

    std::unordered_map<std::string, SensorData> m_sensorStates;
};