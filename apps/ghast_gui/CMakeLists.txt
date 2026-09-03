add_executable(ghast_gui WIN32
    src/main.cpp
)

target_link_libraries(ghast_gui
    PRIVATE
        ghast::windows
        Comctl32
)

set_target_properties(ghast_gui PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/apps/ghast_gui/$<CONFIG>
)

ghast_configure_target(ghast_gui)