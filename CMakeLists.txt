cmake_minimum_required(VERSION 3.14.0)
project(onenet VERSION 0.1.0 LANGUAGES C CXX)

# 1. Set the default CXX flags (C++ compiler)
# Add -g (debug symbols) and -O0 (no optimization) for the Debug build type.
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
# 2. Add debug symbols for 'Release with Debug Info' 
# (Good for optimized code where you still want a stack trace)
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g") 
# 3. Set the standard optimization flags for Release builds
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
# export compile commands
set(CMAKE_EXPORT_COMPILE_COMMANDS "ON")

set(CMAKE_CXX_STANDARD 11)
# Instructs it to manage the C dependency internally
set(PAHO_WITH_MQTT_C ON)
# Builds static targets (better for FetchContent)
set(PAHO_BUILD_STATIC TRUE)
# Prevents shared library build confusion
set(PAHO_BUILD_SHARED FALSE)
# disable tl expected unit test
set(EXPECTED_BUILD_TESTS OFF)
# disable using zstd
set(HTTPLIB_USE_ZSTD_IF_AVAILABLE OFF)
set(
  SRC_FILES
  src/main.cpp
  src/onenet_client.cpp
  src/base64_openssl.cpp
  src/url_util_httplib.cpp
)

set(CL_ONENET_LOG_LEVEL 1 CACHE STRING "set program log level (DEBUG=0, INFO=1, WARN=2, ERROR=3)")

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

# Fetch and make tl::expected available
FetchContent_Declare(
    tl_expected
    GIT_REPOSITORY https://github.com/TartanLlama/expected.git
    GIT_TAG v1.3.1
)
FetchContent_MakeAvailable(tl_expected)

# Fetch and make libcurl available
FetchContent_Declare(
    httplib
    GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
    GIT_TAG v0.27.0
)
FetchContent_MakeAvailable(httplib)

# Fetch and make jsoncpp available
FetchContent_Declare(
    jsoncpp
    GIT_REPOSITORY https://github.com/open-source-parsers/jsoncpp.git
    GIT_TAG 1.9.6
)
FetchContent_MakeAvailable(jsoncpp)

add_executable(onenet ${SRC_FILES})

target_include_directories(onenet PRIVATE ${eclipse-paho-mqtt-cpp_SOURCE_DIR}/externals/paho-mqtt-c/src)
target_include_directories(onenet PRIVATE ${eclipse-paho-mqtt-cpp_SOURCE_DIR}/include)
target_include_directories(onenet PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(onenet PRIVATE ${tl_expected_SOURCE_DIR}/include)
target_include_directories(onenet PRIVATE ${cxxopts_SOURCE_DIR}/include)
target_include_directories(onenet PRIVATE ${jsoncpp_SOURCE_DIR}/include)
target_include_directories(onenet PRIVATE ${fmt_SOURCE_DIR}/include)
target_include_directories(onenet PRIVATE ${httplib_SOURCE_DIR})

target_link_libraries(onenet PUBLIC paho-mqttpp3-static)
target_link_libraries(onenet PUBLIC expected)
target_link_libraries(onenet PUBLIC cxxopts)
target_link_libraries(onenet PUBLIC httplib)
target_link_libraries(onenet PUBLIC jsoncpp)
target_link_libraries(onenet PUBLIC fmt)

target_compile_definitions(onenet 
                           PRIVATE
                           "CL_ONENET_LOG_LEVEL=${CL_ONENET_LOG_LEVEL}")
