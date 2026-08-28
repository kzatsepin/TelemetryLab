qt_add_executable(telemetry_app
    src/main.cpp
)

qt_add_qml_module(telemetry_app
    URI TelemetryLab
    VERSION 1.0
    QML_FILES
        src/Main.qml
)

set_target_properties(telemetry_app PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(telemetry_app
    PRIVATE
        Qt6::Quick
        #telemetry_core
)
