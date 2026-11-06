#include "ECUServer.hpp"
#include "Logger.hpp"
#include "SingleInstanceGuard.hpp"

#include <iostream>

int main()
{   
    SingleInstanceGuard ecuInstanceGuard{"/tmp/automotive_ecu_processor.lock"};
    
    Logger::instance().initialize("logs/ecu_processor.log");
    constexpr const char* socketPath = "/tmp/ecu_processor.sock";

    auto& server = ECUServer::instance();

    if (!server.start(socketPath)){
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

     Logger::instance().info("ECU server stopped");
    return 0;
}