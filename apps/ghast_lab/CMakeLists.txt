add_executable(ghast_lab
    src/main.cpp
)

set_target_properties(ghast_lab PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/apps/ghast_lab/$<CONFIG>
)

ghast_configure_target(ghast_lab)

add_test(NAME ghast_lab_runs COMMAND ghast_lab)
