cmake_minimum_required(VERSION 3.16)
project(aoc C)

# ---- Options / build type ----------------------------------------------------

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)

# Output directories similar to your Makefile.old
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

include_directories(${CMAKE_SOURCE_DIR}/src)

# Common warnings, then tweak per build type
add_compile_options(-Wall -Wextra)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-g -O0)
else()
    add_compile_options(-Werror -O3 -ffast-math -mcpu=apple-m2)
endif()

# ---- lib -----------------------------------------------------------------

find_package(CURL REQUIRED)
find_package(Threads REQUIRED)

# ---- Containers library ------------------------------------------------------

file(GLOB CONTAINER_SRCS CONFIGURE_DEPENDS
        "${CMAKE_SOURCE_DIR}/src/containers/*.c"
)

add_library(containers STATIC ${CONTAINER_SRCS})
target_include_directories(containers PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(containers PUBLIC CURL::libcurl)

# ---- Core library ------------------------------------------------------------

set(CORE_SRCS
        src/aoc_client.c
        src/util.c
        src/runner.c
)

add_library(core STATIC ${CORE_SRCS})
target_include_directories(core PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(core PUBLIC CURL::libcurl)

# ---- Days: one executable per days/<YEAR>_dayNN.c ---------------------------

file(GLOB DAY_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_SOURCE_DIR}/days/*_day*.c"
)

foreach(day_src ${DAY_SOURCES})
    get_filename_component(day_name "${day_src}" NAME_WE) # e.g. 2025_day01

    add_executable(${day_name} "${day_src}")
    target_link_libraries(${day_name} PRIVATE core containers Threads::Threads)
    target_include_directories(${day_name} PRIVATE
            ${CMAKE_SOURCE_DIR}/src
            ${CMAKE_SOURCE_DIR}/days
    )

    add_custom_target(run-${day_name}-part1
            COMMAND $<TARGET_FILE:${day_name}> --part 1
            DEPENDS ${day_name}
            USES_TERMINAL
    )

    add_custom_target(run-${day_name}-part2
            COMMAND $<TARGET_FILE:${day_name}> --part 2
            DEPENDS ${day_name}
            USES_TERMINAL
    )

    add_custom_target(submit-${day_name}-part1
            COMMAND $<TARGET_FILE:${day_name}> --part 1 --submit
            DEPENDS ${day_name}
            USES_TERMINAL
    )

    add_custom_target(submit-${day_name}-part2
            COMMAND $<TARGET_FILE:${day_name}> --part 2 --submit
            DEPENDS ${day_name}
            USES_TERMINAL
    )

    add_custom_target(bench-${day_name}
            COMMAND $<TARGET_FILE:${day_name}> --bench 20000
            DEPENDS ${day_name}
            USES_TERMINAL
    )
endforeach()

# Single meta-target that will run all benchmarks sequentially
add_custom_target(bench-all
        COMMAND ${CMAKE_COMMAND} -E echo "===== Benchmarking all days ====="
        USES_TERMINAL
)

foreach(day_src ${DAY_SOURCES})
    get_filename_component(day_name "${day_src}" NAME_WE)

    # Make sure the executable exists
    add_dependencies(bench-all ${day_name})

    # Append commands to bench-all; these run in order
    add_custom_command(
            TARGET bench-all
            COMMAND ${CMAKE_COMMAND} -E echo "Benchmarking ${day_name}..."
            COMMAND $<TARGET_FILE:${day_name}> --bench 20000
            USES_TERMINAL
    )
endforeach()

# ---- containers_test ---------------------------------------------------------

add_executable(containers_test
        src/tests/containers_test.c
)

target_link_libraries(containers_test PRIVATE containers CURL::libcurl)
target_include_directories(containers_test PRIVATE ${CMAKE_SOURCE_DIR}/src)

include(CTest)
enable_testing()
add_test(NAME containers_test
        COMMAND containers_test)
