#include "sensor_simulator/include/SensorFactory.hpp"

int main() {
    auto sensor = SensorFactory::create(cfg); //need to get from config

    sensor->sample();
    return 0;
}