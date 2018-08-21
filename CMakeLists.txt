cmake_minimum_required(VERSION 3.5)
project(towave VERSION 3.1.0 LANGUAGES CXX)

add_subdirectory(game-music-emu)
find_package(libgme REQUIRED)

add_executable(towave
        towave.cpp
        wave_writer.c
        )
target_link_libraries(towave PRIVATE libgme)

#https://www.youtube.com/watch?v=eC9-iRN2b04 do not reference includes beyond your `project`
#target_include_directories(towave PRIVATE game-music-emu/gme)
