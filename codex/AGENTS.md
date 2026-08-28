# TelemetryLab review rules

- This is a C++/Qt learning project. The developer implements each task; Codex reviews and diagnoses by default.
- Do not modify product code unless the user explicitly asks for implementation or fixes.
- Review the current task in `docs/04_TASKS.md` and its Definition of Done before commenting on later-stage design.
- Prioritize correctness and buildability over formatting. Every finding should include severity, location, failure scenario, and fix direction.
- Run a clean configure/build and relevant tests when possible; report the exact commands and distinguish project failures from missing local tools.
- Keep CMake target-based: declare dependencies with `target_link_libraries` and usage requirements with `target_include_directories`; avoid directory-global include/link settings.
- Every target required by the current task must participate in the top-level build and be buildable by its declared target name.
- Keep the supported CMake and Qt version floor intentional, documented, and no newer than required by the APIs in use.
- Declare QML through `qt_add_qml_module`; do not manually copy QML files that CMake can package and deploy.
- Keep domain/value code free of Qt unless Qt is needed by that layer.
- Qt review priorities: QObject ownership, thread affinity, event-loop blocking, signal delivery, C++/QML ownership, clean shutdown, and deterministic tests.
- Treat warnings, commented-out dependencies, placeholder target names, and generated-template leftovers as unfinished work before marking a task done.
