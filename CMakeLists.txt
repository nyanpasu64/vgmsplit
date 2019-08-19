cmake_minimum_required(VERSION 3.5)
INCLUDE(CheckIncludeFileCXX)

project(vgmsplit VERSION 3.2.0 LANGUAGES CXX C)
if (NOT DEFINED vgmsplit_VERSION)
    message(FATAL_ERROR "version number not working")
endif ()


# Add vgmsplit
add_executable(vgmsplit
        vgmsplit.cpp
        wave_writer.c
        )
target_include_directories(vgmsplit PRIVATE .)
target_include_directories(vgmsplit PRIVATE game-music-emu)
target_compile_definitions(vgmsplit PRIVATE VGMSPLIT_VERSION="${vgmsplit_VERSION}")     # not used in app yet
if (MINGW)
    target_link_libraries(vgmsplit PRIVATE -static-libgcc -static-libstdc++ -static)
endif ()

# Setup <filesystem>
set_property(TARGET vgmsplit PROPERTY CXX_STANDARD 17)

CHECK_INCLUDE_FILE_CXX(filesystem fs_exists)
if (fs_exists)
    # noop
else ()
    CHECK_INCLUDE_FILE_CXX(experimental/filesystem exp_fs_exists)
    if (exp_fs_exists)
        target_compile_definitions(vgmsplit PRIVATE EXPERIMENTAL_FS)
        if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
            target_link_libraries(vgmsplit PRIVATE "-lstdc++fs")
        endif ()
    else ()
        message(FATAL_ERROR "vgmsplit requires <filesystem> or <experimental/filesystem>")
    endif ()
endif ()

# Add GME library
SET(BUILD_SHARED_LIBS OFF
        CACHE BOOL "Build shared GME library")
SET(GME_YM2612_EMU "MAME"
        CACHE STRING "Which YM2612 emulator to use: \"Nuked\" (LGPLv2.1+), \"MAME\" (GPLv2+), or \"GENS\" (LGPLv2.1+)")
add_subdirectory(game-music-emu)


# Link to libgme
add_dependencies(vgmsplit gme)
target_link_libraries(vgmsplit PRIVATE gme)
