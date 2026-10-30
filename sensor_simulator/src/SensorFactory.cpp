#include <memory>
#include <stdexcept>
#include "sensor_simulator/include/ISensor.hpp";
#include "sensor_simulator/include/SensorFactory.hpp";
#include "sensor_simulator/include/sensors/RpmSensor.hpp";
#include "sensor_simulator/include/sensors/SpeedometerSensor.hpp";
#include "sensor_simulator/include/sensors/ClimateSensor.hpp";

std::unique_ptr<ISensor> SensorFactory::create(const SensorConfig& cfg){
    if (cfg.type == "RPM") {
        return std::make_unique<RpmSensor>(cfg);
    }

    else if(cfg.type == "Speedometer") {
        return std::make_unique<SpeedometerSensor>(cfg);
    }

    else if(cfg.type == "Climate") {
        return std::make_unique<ClimateSensor>(cfg);
    }
    
    throw std::runtime_error("Unknown sensor");
}