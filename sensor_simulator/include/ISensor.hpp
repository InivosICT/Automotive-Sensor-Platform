#include "sensor_simulator/include/SensorData.hpp"

class ISensor {
    public:
        virtual SensorData sample() const = 0;
};