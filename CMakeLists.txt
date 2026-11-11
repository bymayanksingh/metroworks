cmake_minimum_required(VERSION 3.14)
project(MetroWorks VERSION 2.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable compiler warnings
if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Core static library
add_library(metroworks_core STATIC
    src/utils.cpp
    src/station.cpp
    src/fare_calculator.cpp
    src/metro_network.cpp
    src/data_loader.cpp
    src/route_planner.cpp
    src/dot_exporter.cpp
)

target_include_directories(metroworks_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# CLI Executable
add_executable(metroworks src/main.cpp)
target_link_libraries(metroworks PRIVATE metroworks_core)

# Tests
enable_testing()
add_executable(metroworks_test
    tests/test_main.cpp
    tests/test_graph.cpp
    tests/test_catalog.cpp
    tests/test_fare.cpp
    tests/test_route_planner.cpp
    tests/test_dot_exporter.cpp
)
target_include_directories(metroworks_test PRIVATE tests)
target_link_libraries(metroworks_test PRIVATE metroworks_core)
add_test(NAME AllTests COMMAND metroworks_test)
