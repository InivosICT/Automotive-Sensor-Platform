#include "CommandLineParser.hpp"
#include <string_view>
#include <stdexcept>

CommandLineOptions CommandLineParser::parse(int argc, char* argv[]) {
    CommandLineOptions options;
    bool configProvided = false;

    for(int index =1; index < argc; ++index){
        const std::string_view argument{argv[index]};

        if(argument == "--help"){
            throw std::runtime_error("Usage: sensor_simulator --config=<path-to-json>");
        }

        if(argument == "--config"){
            if(index +1 >= argc){
                throw std::invalid_argument("--config requires a file path");
            }

            options.configPath = argv[++index];
            configProvided = true;
            continue;
        }

        constexpr std::string_view prefix{"--config="};

        if (argument.substr(0, prefix.size()) == prefix) {
            const std::string_view value = argument.substr(prefix.size());

            if (value.empty()) {
                throw std::invalid_argument("--config path cannot be empty");
            }

            options.configPath = std::string{value};
            configProvided = true;
            continue;
        }

        throw std::invalid_argument("Unknown command-line argument: " +std::string{argument});
    }

    if (!configProvided) {
        throw std::invalid_argument(
            "Missing required argument: --config");
    }
    
    return options;

};