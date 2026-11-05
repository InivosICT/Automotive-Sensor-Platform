#include "ECUServer.hpp"
#include "Logger.hpp"

#include <iostream>

int main()
{
    Logger::instance().initialize("logs/ecu_processor.log");
    constexpr const char* socketPath = "/tmp/ecu_processor.sock";

    ECUServer server{socketPath};

    if (!server.start())
    {
        std::cerr
            << "Failed to start ECU server\n";

        Logger::instance().error("Failed to start ECU server");
        return 1;
    }

    std::cout
        << "ECU listening on "
        << socketPath
        << '\n';
    Logger::instance().info("ECU listening on " + std::string(socketPath));
    server.run();

    return 0;
}