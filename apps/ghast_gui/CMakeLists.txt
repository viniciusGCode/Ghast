add_executable(ghast_gui WIN32
    src/main.cpp
)

set_target_properties(ghast_gui PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/apps/ghast_gui/$<CONFIG>
)

ghast_configure_target(ghast_gui)