cmake_minimum_required(VERSION 3.16)

project(SensorSimulator LANGUAGES CXX)

add_executable(sensor_simulator
    sensor_simulator/app/main.cpp
    sensor_simulator/src/SensorFactory.cpp
    sensor_simulator/src/BaseSensor.cpp
    sensor_simulator/src/CommandLineParser.cpp
    sensor_simulator/src/ConfigLoader.cpp
    sensor_simulator/src/PayloadBuilder.cpp
    sensor_simulator/src/UnixSocketClient.cpp
    sensor_simulator/src/sensors/RpmSensor.cpp
    sensor_simulator/src/sensors/SpeedometerSensor.cpp
    sensor_simulator/src/sensors/ClimateSensor.cpp
)

target_include_directories(sensor_simulator
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/sensor_simulator/include
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/rapidjson/include
)

add_executable(ecu_processor
    ECU/app/processor.cpp
    ECU/ECUServer.cpp)

target_include_directories(ecu_processor
    PRIVATE
         ${CMAKE_CURRENT_SOURCE_DIR}/ECU
         ${CMAKE_CURRENT_SOURCE_DIR}/third_party/rapidjson/include
)

target_compile_features(sensor_simulator
    PRIVATE
        cxx_std_17
)

target_compile_features(ecu_processor
    PRIVATE
        cxx_std_17
)

target_compile_options(sensor_simulator
    PRIVATE
        -Wall
        -Wextra
        -Wpedantic
)

target_compile_options(ecu_processor
    PRIVATE
        -Wall
        -Wextra
        -Wpedantic
)