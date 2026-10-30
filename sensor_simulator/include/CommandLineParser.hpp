#pragma once
#include <filesystem>

struct CommandLineOptions  {
    std::filesystem::path configPath;
};

class CommandLineParser {
    public:
        static CommandLineOptions parse(int argc, char* argv[]);
};