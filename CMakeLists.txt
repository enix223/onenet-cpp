cmake_minimum_required(VERSION 3.14.0)
project(onenet VERSION 0.1.0 LANGUAGES C CXX)

set(CXX_STANDARD 11)
set(PAHO_WITH_MQTT_C ON)         # Instructs it to manage the C dependency internally
set(PAHO_BUILD_STATIC TRUE)      # Builds static targets (better for FetchContent)
set(PAHO_BUILD_SHARED FALSE)     # Prevents shared library build confusion
set(
  SRC_FILES
  src/main.cpp
  src/onenet_client.cpp
)

include(FetchContent)

# Fetch and make Paho C++ available
FetchContent_Declare(
    eclipse-paho-mqtt-cpp
    GIT_REPOSITORY https://github.com/eclipse-paho/paho.mqtt.cpp
    GIT_TAG v1.4.2
)
FetchContent_MakeAvailable(eclipse-paho-mqtt-cpp)

# Fetch and make cxxopts available
FetchContent_Declare(
    cxxopts
    GIT_REPOSITORY https://github.com/jarro2783/cxxopts.git
    GIT_TAG v3.3.1
)
FetchContent_MakeAvailable(cxxopts)

# Fetch and make fmt available
FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 12.1.0
)
FetchContent_MakeAvailable(fmt)

add_executable(onenet ${SRC_FILES})

target_include_directories(onenet PRIVATE ${eclipse-paho-mqtt-cpp_SOURCE_DIR}/externals/paho-mqtt-c/src)
target_include_directories(onenet PRIVATE ${eclipse-paho-mqtt-cpp_SOURCE_DIR}/include)
target_include_directories(onenet PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(onenet PRIVATE ${cxxopts_SOURCE_DIR}/include)
target_include_directories(onenet PRIVATE ${fmt_SOURCE_DIR}/include)

target_link_libraries(onenet PUBLIC paho-mqttpp3-static)
target_link_libraries(onenet PUBLIC cxxopts)
target_link_libraries(onenet PUBLIC fmt)
