#include <memory>
#include <stdexcept>
#include "ISensor.hpp"
#include "SensorFactory.hpp"
#include "sensors/RpmSensor.hpp"
#include "sensors/SpeedometerSensor.hpp"
#include "sensors/ClimateSensor.hpp"

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