cmake_minimum_required(VERSION 3.5.0...3.25)
project(unperplex VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif ()

if (USE_ASAN)
    link_libraries(asan)
    add_compile_options(-fsanitize=address)
endif ()

if (USE_UBSAN)
    link_libraries(ubsan)
    add_compile_options(-fsanitize=undefined)
endif ()

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    add_compile_definitions("DEBUG=1")
else ()
    add_compile_definitions("DEBUG=0")
endif ()

include_directories("include")

# Clay setup
include_directories("external/clay")
add_library(clay STATIC "external/clay/clay.h")
target_compile_definitions(clay PUBLIC CLAY_IMPLEMENTATION)
set_target_properties(clay PROPERTIES LANGUAGE C)
set_target_properties(clay PROPERTIES LINKER_LANGUAGE C)

add_subdirectory("external/ctl")
add_subdirectory("external/raylib")
include_directories("external/raylib/src")

file(GLOB_RECURSE EXEC_SOURCES src/**.c)

add_executable(unperplex_exec ${EXEC_SOURCES})
target_compile_definitions(unperplex_exec PRIVATE CL_TARGET_OPENCL_VERSION=300)
target_link_libraries(unperplex_exec PRIVATE OpenCL raylib ctl_static m clay)
set_target_properties(unperplex_exec PROPERTIES OUTPUT_NAME "unperplexed")
