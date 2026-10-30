#include "SensorFactory.hpp"
#include <iostream>
#include "CommandLineParser.hpp"
#include "ConfigLoader.hpp"

int main(int argc, char* argv[]) {
    try
    {
        const CommandLineOptions options = CommandLineParser::parse(argc, argv);
        std::cout << "Config file path: " << options.configPath << '\n';
        const SimulatorConfig config = ConfigLoader::load(options.configPath);
        std::cout << "Vehicle ID: " << config.vehicleId << '\n';
        std::cout << "ECU Socket Path: " << config.ecuSocketPath << '\n';
        for (const SensorConfig& sensorConfig : config.sensors) {
            std::cout << "Sensor ID: " << sensorConfig.id << '\n';
            std::cout << "Sensor Type: " << sensorConfig.type << '\n';
            std::cout << "Sensor Period (s): " << sensorConfig.periodSec << '\n';
        }
    // auto sensor = SensorFactory::create(cfg); //need to get from config

    // sensor->sample();
    }
     catch (const std::exception& error)
    {
        std::cerr
            << "sensor_simulator: "
            << error.what()
            << '\n';

        return 1;
    }
    return 0;
}