#include "SensorFactory.hpp"
#include "CommandLineParser.hpp"
#include "ConfigLoader.hpp"
#include "PayloadBuilder.hpp"
#include "UnixSocketClient.hpp"
#include "Logger.hpp"

#include <iostream>
#include <atomic>
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>
#include <exception>
#include <string>

int main(int argc, char* argv[]) {
    try
    {
        Logger::instance().initialize("logs/sensor_simulator.log");

        const CommandLineOptions options = CommandLineParser::parse(argc, argv);
        std::cout << "Config file path: " << options.configPath << '\n';
        Logger::instance().info("Config file path: " + options.configPath.string());
        
        const SimulatorConfig config = ConfigLoader::load(options.configPath);
        std::cout << "Vehicle ID: " << config.vehicleId << '\n';
        Logger::instance().info("Vehicle ID: " + config.vehicleId);
        std::cout << "ECU Socket Path: " << config.ecuSocketPath << '\n';
        Logger::instance().info("ECU Socket Path: " + config.ecuSocketPath);

        std::atomic<bool> running{true};
        std::mutex outputMutex;

        std::vector<std::thread> sensorThreads;
        sensorThreads.reserve(config.sensors.size());

        for (const SensorConfig& sensorConfig : config.sensors) {
            std::cout << "Sensor ID: " << sensorConfig.id <<"|"<< "Sensor Type: " << sensorConfig.type <<"|"<< "Sensor Period (s): " << sensorConfig.periodSec<<std::endl;
            Logger::instance().info("Sensor ID: " + sensorConfig.id + "|" + "Sensor Type: " + sensorConfig.type + "|" + "Sensor Period (s): " + std::to_string(sensorConfig.periodSec) + "|" + "Sensor Min Value: " + std::to_string(sensorConfig.minValue) + "|" + "Sensor Max Value: " + std::to_string(sensorConfig.maxValue));
            std::unique_ptr<ISensor> sensor = SensorFactory::create(sensorConfig);

             sensorThreads.emplace_back( [sensor = std::move(sensor), sensorConfig, socketPath = config.ecuSocketPath, &running, &outputMutex]() mutable
                {
                    PayloadBuilder payloadBuilder;
                    UnixSocketClient socketClient{
                        socketPath
                    };

                if (!socketClient.connect()) {
                    std::lock_guard<std::mutex> lock{outputMutex};

                    std::cerr << "Failed to connect sensor "<< sensorConfig.id << " to " << socketPath << '\n';
                    Logger::instance().error("Failed to connect sensor " + sensorConfig.id + " to " + socketPath);
                    return;
                }

                while (running.load())
                    {
                        const SensorData data = sensor->sample();

                        const std::string payload =payloadBuilder.build(data);

                         if (!socketClient.sendMessage(payload)){
            
                            std::lock_guard<std::mutex> lock{outputMutex};

                            Logger::instance().error("Failed to send payload from sensor " + sensorConfig.id);
                            break;
        }

                        {
                            std::lock_guard<std::mutex> lock{outputMutex};

                            std::cout<<"Vehicle ID: "<< sensorConfig.vehicleId << ", Sensor ID: "<< data.sensorId<< ", type: "<< data.sensorType<< ", value: "<< data.value<< ", timestamp: "<< data.timestampMs<< '\n';
                            Logger::instance().info("Vehicle ID: " + sensorConfig.vehicleId + ", Sensor ID: " + data.sensorId + ", type: " + data.sensorType + ", value: " + std::to_string(data.value) + ", timestamp: " + std::to_string(data.timestampMs));
                        }

                        std::this_thread::sleep_for(std::chrono::seconds{sensorConfig.periodSec});
                    }

                    std::lock_guard<std::mutex> lock{outputMutex};

                    Logger::instance().info("Stopped sensor: " + sensorConfig.id);
                });
        }

        std::cout
            << config.sensors.size()
            << " sensor workers started.\n"
            << "Press Enter to stop...\n";

        std::cin.get();

        /*
         * All worker threads observe this flag.
         */
        running.store(false);

        /*
         * Wait until all worker threads have finished.
         */
        for (std::thread& sensorThread : sensorThreads)
        {
            if (sensorThread.joinable())
            {
                sensorThread.join();
            }
        }

        std::cout << "All sensor workers stopped.\n";
        Logger::instance().info("All sensor workers stopped.");
    
    }
     catch (const std::exception& error)
    {
        std::cerr
            << "sensor_simulator: "
            << error.what()
            << '\n';

        Logger::instance().error("sensor_simulator: " + std::string(error.what()));
        return 1;
    }
    return 0;
}