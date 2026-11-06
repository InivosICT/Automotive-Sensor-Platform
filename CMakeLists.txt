cmake_minimum_required(VERSION 3.16)

project(AutomotiveSensorPlatform LANGUAGES CXX)

find_package(Threads REQUIRED)

add_library(logger SHARED 
    shared/logger/Logger.cpp)

target_include_directories(logger
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/shared/logger/include
)

target_compile_features(logger
    PUBLIC
        cxx_std_17
)

target_link_libraries(logger
    PUBLIC
        Threads::Threads
)

target_compile_options(logger
    PRIVATE
        -Wall
        -Wextra
        -Wpedantic
)


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

add_executable(ecu_processor
    ECU/app/processor.cpp
    ECU/ECUServer.cpp
    ECU/SingleInstanceGuard.cpp
    ECU/InternalStateMap.cpp
    ECU/DiagnosticModule.cpp
    ECU/ActuatorProxy.cpp)



target_include_directories(sensor_simulator
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/sensor_simulator/include
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/rapidjson/include
        ${CMAKE_CURRENT_SOURCE_DIR}/shared/data/include
)

target_include_directories(ecu_processor
    PRIVATE
         ${CMAKE_CURRENT_SOURCE_DIR}/ECU/include
         ${CMAKE_CURRENT_SOURCE_DIR}/third_party/rapidjson/include
         ${CMAKE_CURRENT_SOURCE_DIR}/shared/data/include

)

target_link_libraries(sensor_simulator
    PRIVATE
        logger
        )

target_link_libraries(ecu_processor
    PRIVATE
        logger
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