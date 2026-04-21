# SPDX-FileCopyrightText: © 2025 ALIAS Developers
# SPDX-FileCopyrightText: © 2020 Alias Developers
# SPDX-FileCopyrightText: © 2019 SpectreCoin Developers
# SPDX-License-Identifier: MIT
#
# Inspired by The ViaDuck Project for building OpenSSL
#
# CMakeLists.txt - Main CMake configuration for XZ/LZMA library build system
#
# This file sets up the XZ library (liblzma) build configuration, including:
# - Project configuration and minimum CMake version
# - Cache variables for build customization
# - Interface library target for linking
# - External project integration for building XZ from source

cmake_minimum_required(VERSION 3.2)
project(libxz-cmake
    VERSION 1.0.0
    DESCRIPTION "CMake build system for XZ/LZMA library"
    LANGUAGES C
)

# Cache variables for build configuration
set(LIBLZMA_ARCHIVE_LOCATION OFF CACHE STRING "Location of XZLib archive")
set(LIBLZMA_BUILD_VERSION OFF CACHE STRING "XZLib version to build")
set(CROSS_ANDROID OFF CACHE BOOL "Cross-compiling for Android?")
set(CROSS OFF CACHE BOOL "Cross-compiling?")
set(CROSS_TARGET OFF CACHE STRING "Cross-compilation target")

# Allow including our modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

# Create interface library target for linking against liblzma
# This provides a clean interface for consumers of the library
add_library(liblzma INTERFACE)

# Set up imported static library target
# This will be populated with the actual library location after build
add_library(lib_lzma STATIC IMPORTED GLOBAL)

# Link the imported library to the interface target
target_link_libraries(liblzma INTERFACE lib_lzma)

# Set up build prefix and paths
# All build artifacts will be installed under this prefix
set(LIBLZMA_PREFIX ${CMAKE_CURRENT_BINARY_DIR})
message(STATUS "LIBLZMA_PREFIX: ${LIBLZMA_PREFIX}")

# Path to the built liblzma static library
set(LIBLZMA_PATH ${LIBLZMA_PREFIX}/usr/local/lib/liblzma.a)
message(STATUS "LIBLZMA_PATH ${LIBLZMA_PATH}")

# Include the build script that handles the actual XZ library build process
include(BuildXZLib)

# Set the imported library location property
# This tells CMake where to find the built library
set_target_properties(lib_lzma PROPERTIES IMPORTED_LOCATION ${LIBLZMA_PATH})

# Set include directories for consumers of the liblzma target
target_include_directories(liblzma BEFORE INTERFACE ${LIBLZMA_PREFIX}/usr/local/include)

# Ensure the external project is built before the imported target is used
add_dependencies(lib_lzma libxzExternal)
