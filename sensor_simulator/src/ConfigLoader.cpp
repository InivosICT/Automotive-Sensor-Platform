#include "ConfigLoader.hpp"
#include <fstream>
#include <iostream>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>


SimulatorConfig ConfigLoader::load(const std::filesystem::path& configPath) {

    if (!std::filesystem::exists(configPath)) { 
        throw std::runtime_error("Config file does not exist: " + configPath.string());
    }

    std::ifstream input{ configPath, std::ios::in | std::ios::binary};

     if (!input.is_open()) {
        throw std::runtime_error("Could not open configuration file: " + configPath.string());
    }
    std::cout<< "Loading configuration from: " << configPath << '\n';

    rapidjson::IStreamWrapper inputStream{input};
    rapidjson::Document document;
    document.ParseStream(inputStream);

    if (document.HasParseError()) {
        throw std::runtime_error("Failed to parse configuration file: " + configPath.string());
    }

    SimulatorConfig config;

    config.vehicleId = document["vehicleId"].GetString();
    config.ecuSocketPath = document["ecuSocketPath"].GetString();
    
    for (const rapidjson::Value& sensorJson : document["sensors"].GetArray()){

        if (!sensorJson.IsObject()){
        throw std::runtime_error("Each sensor must be a JSON object");
    }

        SensorConfig sensorConfig;
        sensorConfig.id = sensorJson["id"].GetString();
        sensorConfig.type = sensorJson["type"].GetString();
        sensorConfig.periodSec = sensorJson["periodSec"].GetInt();

        config.sensors.push_back(sensorConfig);
    }

    return config;
}