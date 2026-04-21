# SPDX-FileCopyrightText: © 2025 ALIAS Developers
# SPDX-FileCopyrightText: © 2020 Alias Developers
# SPDX-FileCopyrightText: © 2019 SpectreCoin Developers
# SPDX-License-Identifier: MIT
#
# Inspired by The ViaDuck Project for building OpenSSL
#
# CMakeLists.txt - Main CMake configuration for BerkeleyDB library build system
#
# This file sets up the BerkeleyDB library build configuration, including:
# - Project configuration and minimum CMake version
# - Cache variables for build customization
# - Interface library target for linking
# - External project integration for building BerkeleyDB from source

cmake_minimum_required(VERSION 3.2)
project(berkeleydb-cmake
    VERSION 1.0.0
    DESCRIPTION "CMake build system for BerkeleyDB library"
    LANGUAGES C CXX
)

# Cache variables for build configuration
set(BERKELEYDB_ARCHIVE_LOCATION OFF CACHE STRING "Location of BerkeleyDB archive")
set(BERKELEYDB_BUILD_VERSION OFF CACHE STRING "BerkeleyDB version to build")
set(CROSS_ANDROID OFF CACHE BOOL "Cross-compiling for Android?")
set(CROSS OFF CACHE BOOL "Cross-compiling?")
set(CROSS_TARGET OFF CACHE STRING "Cross-compilation target")

# Allow including our modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

# Create interface library target for linking against BerkeleyDB
# This provides a clean interface for consumers of the library
add_library(bdb INTERFACE)

# Set up imported static library target
# This will be populated with the actual library location after build
add_library(bdb_lib STATIC IMPORTED GLOBAL)

# Link the imported library to the interface target
target_link_libraries(bdb INTERFACE bdb_lib)

# Set up build prefix and paths
# All build artifacts will be installed under this prefix
set(BERKELEYDB_PREFIX ${CMAKE_CURRENT_BINARY_DIR})
message(STATUS "BERKELEYDB_PREFIX: ${BERKELEYDB_PREFIX}")

# Path to the built BerkeleyDB C++ library
set(BERKELEYDB_LIBDB_PATH ${BERKELEYDB_PREFIX}/usr/local/BerkeleyDB.${BERKELEYDB_BUILD_VERSION_SHORT}/lib/libdb_cxx.a)
message(STATUS "BERKELEYDB_LIBDB_PATH: ${BERKELEYDB_LIBDB_PATH}")

# Include the build script that handles the actual BerkeleyDB build process
include(BuildBerkeleyDB)

# Set the imported library location property
# This tells CMake where to find the built library
set_target_properties(bdb_lib PROPERTIES IMPORTED_LOCATION ${BERKELEYDB_LIBDB_PATH})

# Set include directories for consumers of the bdb target
target_include_directories(bdb BEFORE INTERFACE ${BERKELEYDB_PREFIX}/usr/local/BerkeleyDB.${BERKELEYDB_BUILD_VERSION_SHORT}/include)

# Ensure the external project is built before the imported target is used
add_dependencies(bdb_lib berkeleydb)
