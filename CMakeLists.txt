cmake_minimum_required(VERSION 3.25)

project(
    Ghast
    VERSION 0.1.0
    DESCRIPTION "Windows runtime memory exploration toolkit"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

enable_testing()

function(ghast_configure_target target_name)
    target_compile_features(${target_name} PUBLIC cxx_std_20)

    if(MSVC)
        target_compile_options(${target_name} PRIVATE /W4 /permissive- /Zc:__cplusplus)
    else()
        target_compile_options(${target_name} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endfunction()

add_subdirectory(src/windows)

add_subdirectory(apps/ghast_app)
add_subdirectory(apps/ghast_lab)
