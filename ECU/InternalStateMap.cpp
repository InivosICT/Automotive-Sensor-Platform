#include "InternalStateMap.hpp"

#include <mutex>
#include <shared_mutex>

void InternalStateMap::update(const SensorData& sensorData){
    std::unique_lock<std::shared_mutex> lock{m_mutex};
    m_sensorStates[sensorData.sensorId] = sensorData;
}

std::optional<SensorData> InternalStateMap::getById(const std::string& sensorId) const {
    std::shared_lock<std::shared_mutex> lock{m_mutex};

    const auto iterator = m_sensorStates.find(sensorId);

    if (iterator == m_sensorStates.end()) {
        return std::nullopt;
    }

    return iterator->second;
}

std::vector<SensorData>
InternalStateMap::snapshot() const
{
    std::shared_lock<std::shared_mutex> lock{m_mutex };

    std::vector<SensorData> result;
    result.reserve(m_sensorStates.size());

    for (const auto& [sensorId, sensorData] : m_sensorStates) {
        result.push_back(sensorData);
    }

    return result;
}