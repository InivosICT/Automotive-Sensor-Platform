#include <memory>
#include <stdexcept>
#include "ISensor.hpp"
#include "SensorFactory.hpp"
#include "sensors/RpmSensor.hpp"
#include "sensors/SpeedometerSensor.hpp"
#include "sensors/ClimateSensor.hpp"

std::unique_ptr<ISensor> SensorFactory::create(const SensorConfig& cfg){
    if (cfg.type == "RPM") {
        return std::make_unique<RpmSensor>();
    }

    else if(cfg.type == "Speedometer") {
        return std::make_unique<SpeedometerSensor>();
    }

    else if(cfg.type == "Climate") {
        return std::make_unique<ClimateSensor>();
    }
    
    throw std::runtime_error("Unknown sensor");
}