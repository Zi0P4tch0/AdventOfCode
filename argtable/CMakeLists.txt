cmake_minimum_required(VERSION 3.17)
project(AdventOfCode C)

set(CMAKE_C_STANDARD 99)

set(SOURCES
        ${SOURCES}
        ${CMAKE_CURRENT_SOURCE_DIR}/argtable3.c
        PARENT_SCOPE)

set(HEADERS
        ${HEADERS}
        ${CMAKE_CURRENT_SOURCE_DIR}/argtable3.h
        PARENT_SCOPE)