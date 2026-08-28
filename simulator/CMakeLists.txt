add_executable(telemetry_simulator
    src/main.cpp
)

find_package(Qt6 REQUIRED COMPONENTS Core)

target_link_libraries(telemetry_simulator
    PRIVATE
        Qt6::Core
)
