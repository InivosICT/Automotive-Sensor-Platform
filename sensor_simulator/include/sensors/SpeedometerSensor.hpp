#include "sensor_simulator/include/ISensor.hpp"
class SpeedometerSensor : public ISensor {
    public:
        SensorData sample() const override;
};