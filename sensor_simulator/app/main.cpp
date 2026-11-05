#include "SensorFactory.hpp"
#include "CommandLineParser.hpp"
#include "ConfigLoader.hpp"
#include "PayloadBuilder.hpp"
#include "UnixSocketClient.hpp"

#include <iostream>
#include <atomic>
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>

int main(int argc, char* argv[]) {
    try
    {
        const CommandLineOptions options = CommandLineParser::parse(argc, argv);
        std::cout << "Config file path: " << options.configPath << '\n';
        
        const SimulatorConfig config = ConfigLoader::load(options.configPath);
        std::cout << "Vehicle ID: " << config.vehicleId << '\n';
        std::cout << "ECU Socket Path: " << config.ecuSocketPath << '\n';

        std::atomic<bool> running{true};
        std::mutex outputMutex;

        std::vector<std::thread> sensorThreads;
        sensorThreads.reserve(config.sensors.size());

        for (const SensorConfig& sensorConfig : config.sensors) {
            std::cout << "Sensor ID: " << sensorConfig.id <<"|"<< "Sensor Type: " << sensorConfig.type <<"|"<< "Sensor Period (s): " << sensorConfig.periodSec<<std::endl;
            
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

                    return;
                }

                while (running.load())
                    {
                        const SensorData data = sensor->sample();

                        const std::string payload =payloadBuilder.build(data);

                         if (!socketClient.sendMessage(payload)){
            
                            std::lock_guard<std::mutex> lock{outputMutex};

                            std::cerr << "Failed to send payload from "<< sensorConfig.id << '\n';

                            break;
        }

                        {
                            std::lock_guard<std::mutex> lock{outputMutex};

                            std::cout<<"Vehicle ID: "<< sensorConfig.vehicleId << ", Sensor ID: "<< data.sensorId<< ", type: "<< data.sensorType<< ", value: "<< data.value<< ", timestamp: "<< data.timestampMs<< '\n';
                        }

                        std::this_thread::sleep_for(std::chrono::seconds{sensorConfig.periodSec});
                    }

                    std::lock_guard<std::mutex> lock{outputMutex};

                    std::cout<< "Stopped sensor: "<< sensorConfig.id<< '\n';
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