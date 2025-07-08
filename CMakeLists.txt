cmake_minimum_required(VERSION 3.5.0...3.25)
project(unperplex VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif ()

if (NOT DEFINED UI_HOT_RELOADING)
    set(UI_HOT_RELOADING OFF)
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
    add_compile_definitions("UNPERPLEX_DEBUG=1")
else ()
    add_compile_definitions("UNPERPLEX_DEBUG=0")
endif ()

add_compile_definitions(CL_TARGET_OPENCL_VERSION=300)

include_directories("include")

# Clay setup
include_directories("external/clay")
add_library(clay SHARED "external/clay.c")

add_subdirectory("external/ctl" EXCLUDE_FROM_ALL)
include_directories("external/ctl/include")

set(BUILD_SHARED_LIBS ON)
add_subdirectory("external/raylib" EXCLUDE_FROM_ALL)
include_directories("external/raylib/src")

file(GLOB_RECURSE ALL_C_FILES src/**.c)

if (NOT UI_HOT_RELOADING)
    set(EXEC_SOURCES ${ALL_C_FILES})
else ()
    set(EXEC_SOURCES "${CMAKE_SOURCE_DIR}/src/main.c")
    set(UI_SOURCES)
    set(MAIN_SOURCES)

    foreach (filename IN LISTS ALL_C_FILES)
        if (NOT filename MATCHES "src/ui/components.c"
        AND NOT filename MATCHES "src/ui/layout.c")
            list(APPEND MAIN_SOURCES ${filename})
        elseif (NOT filename MATCHES "src/main.c")
            list(APPEND UI_SOURCES ${filename})
        endif ()
    endforeach ()

    add_library(unperplex_ui SHARED ${UI_SOURCES})
    add_library(unperplex_main SHARED ${MAIN_SOURCES})
    target_link_libraries(unperplex_ui PRIVATE unperplex_main)
    add_compile_definitions("UNPERPLEX_UI_HOT_RELOADING")
endif ()

if (UI_HOT_RELOADING)
    set(LIB_CTL ctl_shared)
else ()
    set(LIB_CTL ctl_static)
endif ()

set(UNPERPLEX_LIBS OpenCL raylib ${LIB_CTL} m clay)

add_executable(unperplex_exec ${EXEC_SOURCES})
target_link_libraries(unperplex_exec PRIVATE ${UNPERPLEX_LIBS})
set_target_properties(unperplex_exec PROPERTIES OUTPUT_NAME "unperplex")

if (UI_HOT_RELOADING)
    target_link_libraries(unperplex_ui PRIVATE ${UNPERPLEX_LIBS})
    target_link_libraries(unperplex_exec PRIVATE unperplex_main)
endif ()
