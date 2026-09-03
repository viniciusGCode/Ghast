add_executable(ghast_app
    src/main.cpp
)

target_link_libraries(ghast_app
    PRIVATE
        ghast::windows
)

set_target_properties(ghast_app PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/apps/ghast_app/$<CONFIG>
)

ghast_configure_target(ghast_app)

add_test(NAME ghast_app_runs COMMAND ghast_app)
