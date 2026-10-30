#include <memory>
#include "sensor_simulator/include/ISensor.hpp";
#include "sensor_simulator/include/SensorConfig.hpp";

class SensorFactory {
    public:
        static std::unique_ptr<ISensor> create(const SensorConfig&);
};