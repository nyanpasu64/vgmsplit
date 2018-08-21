cmake_minimum_required(VERSION 3.5)
project(towave VERSION 3.1.0 LANGUAGES CXX C)

SET(BUILD_SHARED_LIBS OFF CACHE BOOL "Build GME shared library")
add_subdirectory(game-music-emu)

add_executable(towave
        towave.cpp
        wave_writer.c
        )
target_include_directories(towave PRIVATE game-music-emu)

# Link to libgme
add_dependencies(towave gme)

#https://stackoverflow.com/a/33460749/
#message(${CMAKE_CURRENT_BINARY_DIR})
#find_library(gme libgme.a PATHS ${CMAKE_CURRENT_BINARY_DIR}/game-music-emu/gme)
target_link_libraries(towave PRIVATE gme)
