cmake_minimum_required(VERSION 3.5)
project(towave VERSION 3.1.0 LANGUAGES CXX C)


# Add towave
add_executable(towave
        towave.cpp
        wave_writer.c
        )
target_include_directories(towave PRIVATE game-music-emu)


# Add GME library
SET(BUILD_SHARED_LIBS OFF
        CACHE BOOL "Build shared GME library")
SET(GME_YM2612_EMU "MAME"
        CACHE BOOL "Which YM2612 emulator to use: \"Nuked\" (LGPLv2.1+), \"MAME\" (GPLv2+), or \"GENS\" (LGPLv2.1+)")
add_subdirectory(game-music-emu)


# Link to libgme
add_dependencies(towave gme)
target_link_libraries(towave PRIVATE gme)
