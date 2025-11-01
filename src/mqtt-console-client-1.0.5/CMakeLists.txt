cmake_minimum_required(VERSION 3.10)
project(mqtt_client_cpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PahoMqttCpp REQUIRED)
find_package(nlohmann_json REQUIRED)

add_executable(mqtt_client main.cpp)

target_link_libraries(mqtt_client
    PahoMqttCpp::paho-mqttpp3
    nlohmann_json::nlohmann_json
)

