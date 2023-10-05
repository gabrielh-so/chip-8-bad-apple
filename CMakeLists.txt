cmake_minimum_required(VERSION 3.16)

set (CMAKE_CXX_STANDARD 20)

SET (CMAKE_CXX_FLAGS " -g -Wall -O0")

project(chip-8 VERSION 1.0)
include (FindPkgConfig)
file (GLOB SOURCES "*.cpp" "*.c")

add_executable(chip-8 ${SOURCES})

pkg_search_module (SDL2 REQUIRED sdl2)

target_link_options(chip-8 PRIVATE -Wl,--no-as-needed -ldl -lm -lX11 -pthread -lSDL2)

target_include_directories(chip-8 PRIVATE ${CMAKE_SOURCE_DIR}/include ${SDL2_INCLUDE_DIRS})
