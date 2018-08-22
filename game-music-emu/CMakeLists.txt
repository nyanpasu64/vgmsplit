# CMake project definition file.
project(libgme)
cmake_minimum_required(VERSION 3.7 FATAL_ERROR)


include (CheckCXXCompilerFlag)

# When version is changed, also change the one in gme/gme.h to match
set(GME_VERSION 0.6.2 CACHE INTERNAL "libgme Version")

# Setup target cxx flags.

# Default emulators to build (all of them! ;)
if (NOT DEFINED USE_GME_AY)
    SET(USE_GME_AY 1 CACHE BOOL "Enable support for Spectrum ZX music emulation")
endif()

if (NOT DEFINED USE_GME_GBS)
    SET(USE_GME_GBS 1 CACHE BOOL "Enable support for Game Boy music emulation")
endif()

if (NOT DEFINED USE_GME_GYM)
    SET(USE_GME_GYM 1 CACHE BOOL "Enable Sega MegaDrive/Genesis music emulation")
endif()

if (NOT DEFINED USE_GME_HES)
    SET(USE_GME_HES 1 CACHE BOOL "Enable PC Engine/TurboGrafx-16 music emulation")
endif()

if (NOT DEFINED USE_GME_KSS)
    SET(USE_GME_KSS 1 CACHE BOOL "Enable MSX or other Z80 systems music emulation")
endif()

if (NOT DEFINED USE_GME_NSF)
    SET(USE_GME_NSF 1 CACHE BOOL "Enable NES NSF music emulation")
endif()

if (NOT DEFINED USE_GME_NSFE)
    SET(USE_GME_NSFE 1 CACHE BOOL "Enable NES NSFE and NSF music emulation")
endif()

if (NOT DEFINED USE_GME_SAP)
    SET(USE_GME_SAP 1 CACHE BOOL "Enable Atari SAP music emulation")
endif()

if (NOT DEFINED USE_GME_SPC)
    SET(USE_GME_SPC 1 CACHE BOOL "Enable SNES SPC music emulation")
endif()

if (NOT DEFINED USE_GME_VGM)
    SET(USE_GME_VGM 1 CACHE BOOL "Enable Sega VGM/VGZ music emulation")
endif()

if (NOT DEFINED GME_YM2612_EMU)
    SET(GME_YM2612_EMU "Nuked" CACHE STRING "Which YM2612 emulator to use: \"Nuked\" (LGPLv2.1+), \"MAME\" (GPLv2+), or \"GENS\" (LGPLv2.1+)")
endif()

if (USE_GME_NSFE AND NOT USE_GME_NSF)
    MESSAGE(" -- NSFE support requires NSF, enabling NSF support. --")
    SET(USE_GME_NSF 1 CACHE BOOL "Enable NES NSF music emulation" FORCE)
endif()

option(BUILD_SHARED_LIBS "Build shared library (set to OFF for static library)" ON)

option(ENABLE_UBSAN "Enable Undefined Behavior Sanitizer error-checking" ON)


set(cxx_flags "")
#set(link_flags "")

# Check for GCC/Clang "visibility" support.
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU"
    OR
    CMAKE_CXX_COMPILER_ID STREQUAL "Clang")

    set (cxx_flags "${cxx_flags} -Wall -W -Wextra")
    set (cxx_flags "${cxx_flags} -std=c++11")

    # Assume we have visibility support on any compiler that supports C++11
    add_definitions (-DLIBGME_VISIBILITY)
    set (cxx_flags "${cxx_flags} -fvisibility=hidden -fvisibility-inlines-hidden")

    # Try to protect against undefined behavior from signed integer overflow
    # This has caused miscompilation of code already and there are other
    # potential uses; see https://bitbucket.org/mpyne/game-music-emu/issues/18/
    set (cxx_flags "${cxx_flags} -fwrapv")

    if (NOT DEFINED LIBGME_SWITCH_FALLTHROUGH)
        check_cxx_compiler_flag (-Wimplicit-fallthrough __LIBGME_SWITCH_FALLTHROUGH_WARNINGS)
        set (LIBGME_SWITCH_FALLTHROUGH ${__LIBGME_SWITCH_FALLTHROUGH_WARNINGS}
            CACHE BOOL "Set if the compiler will complain about implicit switch fallthrough"
            )
    endif()

    if (ENABLE_UBSAN)
        # GCC needs -static-libubsan
        if (NOT BUILD_SHARED_LIBS AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            set (cxx_flags "${cxx_flags} -fsanitize=undefined -static-libubsan")
        else()
            set (cxx_flags "${cxx_flags} -fsanitize=undefined")
        endif()
    endif()
endif ()

if(LIBGME_SWITCH_FALLTHROUGH)
     # Avoid warning spam about switch fallthroughs, which are numerous in
     # the codebase.
     set (cxx_flags "${cxx_flags} -Wimplicit-fallthrough=0")
endif()


## Setup targets

# Shared library defined here
add_subdirectory(gme)

# EXCLUDE_FROM_ALL adds build rules but keeps it out of default build
add_subdirectory(player EXCLUDE_FROM_ALL)
add_subdirectory(demo EXCLUDE_FROM_ALL)


# Find all targets, to add cxx flags. http://fabic.net/notes/2017/12/22/cmake-random-notes/
get_property(subdirs_list
        DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        PROPERTY SUBDIRECTORIES)
set(targets "")
foreach(subdir ${subdirs_list})
    get_property(_targets_list
            DIRECTORY "${subdir}"
            PROPERTY BUILDSYSTEM_TARGETS )
    list(APPEND targets ${_targets_list})
endforeach()


if(${cxx_flags})
    foreach(target ${targets})
        set_target_properties(${target} PROPERTIES COMPILE_FLAGS ${cxx_flags})
    endforeach(target)
endif()
