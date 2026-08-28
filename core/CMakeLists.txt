add_library(telemetry_core STATIC
    src/main.cpp
    src/file.h
)

target_include_directories(telemetry_core PUBLIC include)

target_link_libraries(telemetry_core PRIVATE Qt6::Core)
