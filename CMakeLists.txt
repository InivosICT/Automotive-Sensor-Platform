cmake_minimum_required(VERSION 3.16)

project(SensorSimulator LANGUAGES CXX)

add_executable(sensor_simulator
    sensor_simulator/app/main.cpp
    sensor_simulator/src/SensorFactory.cpp
    sensor_simulator/src/CommandLineParser.cpp
    sensor_simulator/src/ConfigLoader.cpp
    sensor_simulator/src/sensors/RpmSensor.cpp
    sensor_simulator/src/sensors/SpeedometerSensor.cpp
    sensor_simulator/src/sensors/ClimateSensor.cpp
)

target_include_directories(sensor_simulator
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/sensor_simulator/include
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/rapidjson/include
)

target_compile_features(sensor_simulator
    PRIVATE
        cxx_std_17
)

target_compile_options(sensor_simulator
    PRIVATE
        -Wall
        -Wextra
        -Wpedantic
)