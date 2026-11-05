#include "ECUServer.hpp"

#include <iostream>

int main()
{
    constexpr const char* socketPath = "/tmp/ecu_processor.sock";

    ECUServer server{socketPath};

    if (!server.start())
    {
        std::cerr
            << "Failed to start ECU server\n";

        return 1;
    }

    std::cout
        << "ECU listening on "
        << socketPath
        << '\n';

    server.run();

    return 0;
}