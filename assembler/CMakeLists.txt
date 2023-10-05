cmake_minimum_required(VERSION 3.16)

SET (CMAKE_CXX_FLAGS " -g -Wall")

project(chip-8-assembler VERSION 1.0)
include (FindPkgConfig)
file (GLOB SOURCES "*.cpp" "*.c")

add_executable(chip-8-assembler ${SOURCES})

target_link_options(chip-8-assembler PRIVATE -Wl,--no-as-needed -ldl -lm -pthread)

target_include_directories(chip-8-assembler PRIVATE ${CMAKE_SOURCE_DIR}/include)
