# SPDX-FileCopyrightText: © 2025 ALIAS Developers
# SPDX-FileCopyrightText: © 2020 Alias Developers
# SPDX-FileCopyrightText: © 2019 SpectreCoin Developers
# SPDX-License-Identifier: MIT
#
# Inspired by The ViaDuck Project for building OpenSSL
#
# CMakeLists.txt - Main CMake configuration for Tor library build system
#
# This file sets up the Tor library build configuration, including:
# - Project configuration and minimum CMake version
# - Cache variables for build customization
# - Interface library target for linking
# - External project integration for building Tor from source

cmake_minimum_required(VERSION 3.2)
project(tor-cmake
    VERSION 1.0.0
    DESCRIPTION "CMake build system for Tor library"
    LANGUAGES C
)

# Cache variables for build configuration
set(TOR_ARCHIVE_LOCATION OFF CACHE STRING "Location of Tor archive")
set(TOR_BUILD_VERSION OFF CACHE STRING "Tor version to build")
set(CROSS_ANDROID OFF CACHE BOOL "Cross-compiling for Android?")
set(CROSS OFF CACHE BOOL "Cross-compiling?")
set(CROSS_TARGET OFF CACHE STRING "Cross-compilation target")

# Allow including our modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

# Create interface library target for linking against Tor
# This provides a clean interface for consumers of the library
add_library(libtor INTERFACE)

# Set up imported static library target
# This will be populated with the actual library location after build
add_library(lib_tor STATIC IMPORTED GLOBAL)

# Link the imported library to the interface target
target_link_libraries(libtor INTERFACE lib_tor)

# Set up build prefix and paths
# All build artifacts will be installed under this prefix
set(TOR_LIBTOR_PREFIX ${CMAKE_CURRENT_BINARY_DIR})
message(STATUS "TOR_LIBTOR_PREFIX: ${TOR_LIBTOR_PREFIX}")

# Path to the built Tor binary/library
set(TOR_LIBTOR_PATH ${TOR_LIBTOR_PREFIX}/usr/local/bin/tor)
message(STATUS "TOR_LIBTOR_PATH ${TOR_LIBTOR_PATH}")

# Include the build script that handles the actual Tor build process
include(BuildTor)

# Set the imported library location property
# This tells CMake where to find the built library
set_target_properties(lib_tor PROPERTIES IMPORTED_LOCATION ${TOR_LIBTOR_PATH})

# Set include directories for consumers of the libtor target
target_include_directories(libtor BEFORE INTERFACE ${TOR_LIBTOR_PREFIX}/usr/local/include)

# Note: Additional include paths can be added here if needed
#set(TOR_INCLUDE_PATH
#        ${TOR_LIBTOR_PREFIX}/libtorExternal-prefix/src/libtorExternal/
#        ${TOR_LIBTOR_PREFIX}/libtorExternal-prefix/src/libtorExternal/src/app/main/
#        ${TOR_LIBTOR_PREFIX}/libtorExternal-prefix/src/libtorExternal/src/feature/api/
#        PARENT_SCOPE
#        )

# Ensure the external project is built before the imported target is used
add_dependencies(lib_tor libtorExternal)
