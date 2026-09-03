add_library(ghast_windows
    src/process_snapshot.cpp
    src/process_memory.cpp
)

add_library(ghast::windows ALIAS ghast_windows)

target_include_directories(ghast_windows
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

ghast_configure_target(ghast_windows)
