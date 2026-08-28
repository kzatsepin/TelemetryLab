cmake_minimum_required(VERSION 3.16)
project(TelemetryLab VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick)
qt_standard_project_setup(REQUIRES 6.7)

include(CTest)

add_subdirectory(app)
add_subdirectory(simulator)
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
# add_subdirectory(core)
