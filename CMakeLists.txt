cmake_minimum_required(VERSION 3.17)
project(AdventOfCode C)

set(CMAKE_C_FLAGS_RELEASE "-O2")

find_package (PkgConfig REQUIRED)
pkg_check_modules (GLIB REQUIRED glib-2.0)

include_directories (${GLIB_INCLUDE_DIRS} argtable)
link_directories (${GLIB_LIBRARY_DIRS})
add_definitions (${GLIB_CFLAGS_OTHER} -Wall -Wextra -std=c99 -pedantic)

set(HEADERS ${HEADERS})

set(SOURCES ${SOURCES}
        ${CMAKE_CURRENT_SOURCE_DIR}/main.c
        ${CMAKE_CURRENT_SOURCE_DIR}/combinations.c
        ${CMAKE_CURRENT_SOURCE_DIR}/aoc2020_day1.c
        ${CMAKE_CURRENT_SOURCE_DIR}/aoc2020_day13.c
        ${CMAKE_CURRENT_SOURCE_DIR}/aoc2020_day14.c)

add_subdirectory(argtable)

add_executable(AdventOfCode ${SOURCES} ${HEADERS})

target_link_libraries (AdventOfCode ${GLIB_LIBRARIES})
