find_package(Qt6 REQUIRED COMPONENTS Test)

add_executable(telemetry_tests
    src/test.cpp
)

target_link_libraries(telemetry_tests
    PRIVATE
        Qt6::Test
        #telemetry_core
)


add_test(NAME TelemetrySmokeTest COMMAND telemetry_tests)
