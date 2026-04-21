# SPDX-FileCopyrightText: © 2025 ALIAS Developers
# SPDX-FileCopyrightText: © 2020 Alias Developers
# SPDX-FileCopyrightText: © 2019 SpectreCoin Developers
# SPDX-License-Identifier: MIT
#
# Inspired by The ViaDuck Project for building OpenSSL
#
# BuildXZLib.cmake - CMake module for building XZ/LZMA library from source
#
# This module handles the complete build process for XZ library, including:
# - Platform detection (Windows, Unix, Android cross-compilation)
# - Build tool detection (make, MSYS bash for Windows)
# - Configuration of build parameters for different platforms
# - External project setup for downloading and building XZ
# - Environment setup for cross-compilation scenarios

include(ProcessorCount)
include(ExternalProject)

# Find required dependencies
find_package(Git REQUIRED)
# Use modern Python3 find module (PythonInterp is deprecated)
find_package(Python3 COMPONENTS Interpreter REQUIRED)

# Find patch utility (required for applying patches)
find_program(PATCH_PROGRAM patch)
if (NOT PATCH_PROGRAM)
    message(FATAL_ERROR "Cannot find patch utility. This is only required for Android cross-compilation but due to script complexity "
            "the requirement is always enforced")
endif()

# Determine number of parallel jobs for building
ProcessorCount(NUM_JOBS)
if (NUM_JOBS EQUAL 0)
    set(NUM_JOBS 1)
endif()

# Default to Unix-like OS, will be overridden for Windows/Android
set(OS "UNIX")

# Set up archive hash verification if provided
if (LIBXZ_ARCHIVE_HASH)
    set(LIBLZMA_CHECK_HASH URL_HASH SHA256=${LIBXZ_ARCHIVE_HASH})
endif()

# Skip build if library already exists
if (EXISTS ${LIBLZMA_PATH})
    message(STATUS "Not building XZLib again. Remove ${LIBLZMA_PATH} for rebuild")
else()
    # Platform-specific build tool detection
    if (WIN32 AND NOT CROSS)
        # Windows needs special treatment, but neither cygwin nor msys, since they provide an UNIX-like environment
        if (MINGW)
            set(OS "WIN32")
            message(WARNING "Building on windows is experimental")

            # Find MSYS bash for executing Unix-like commands
            find_program(MSYS_BASH "bash.exe" 
                PATHS "C:/Msys/" "C:/MinGW/msys/" 
                PATH_SUFFIXES "/1.0/bin/" "/bin/"
                DOC "Path to MSYS installation"
            )
            if (NOT MSYS_BASH)
                message(FATAL_ERROR "Specify MSYS installation path")
            endif()

            set(MINGW_MAKE ${CMAKE_MAKE_PROGRAM})
            message(WARNING "Assuming your make program is a sibling of your compiler (resides in same directory)")
        elseif(NOT (CYGWIN OR MSYS))
            message(FATAL_ERROR "Unsupported compiler infrastructure")
        endif()

        set(MAKE_PROGRAM ${CMAKE_MAKE_PROGRAM})
    elseif(NOT UNIX)
        message(FATAL_ERROR "Unsupported platform")
    else()
        # On Unix-like systems, we can only use GNU make, no exotic things like Ninja
        # (MSYS always uses GNU make)
        find_program(MAKE_PROGRAM make)
        if (NOT MAKE_PROGRAM)
            message(FATAL_ERROR "Could not find 'make' program")
        endif()
    endif()

    # save old git values for core.autocrlf and core.eol
    #execute_process(COMMAND ${GIT_EXECUTABLE} config --global --get core.autocrlf OUTPUT_VARIABLE GIT_CORE_AUTOCRLF OUTPUT_STRIP_TRAILING_WHITESPACE)
    #execute_process(COMMAND ${GIT_EXECUTABLE} config --global --get core.eol OUTPUT_VARIABLE GIT_CORE_EOL OUTPUT_STRIP_TRAILING_WHITESPACE)

    # On windows we need to replace path to perl since CreateProcess(..) cannot handle unix paths
    if (WIN32 AND NOT CROSS)
        set(PERL_PATH_FIX_INSTALL sed -i -- 's/\\/usr\\/bin\\/perl/perl/g' Makefile)
    else()
        set(PERL_PATH_FIX_INSTALL true)
    endif()

    # CROSS and CROSS_ANDROID cannot both be set (because of internal reasons)
    if (CROSS AND CROSS_ANDROID)
        # if user set CROSS_ANDROID and CROSS we assume he wants CROSS_ANDROID, so set CROSS to OFF
        set(CROSS OFF)
    endif()

    if (CROSS_ANDROID)
        set(OS "LINUX_CROSS_ANDROID")
    endif()

    # Python helper script for correct building environment
    # This script sets up the proper environment for building on different platforms
    set(BUILD_ENV_TOOL ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/scripts/building_env.py ${OS} ${MSYS_BASH} ${MINGW_MAKE})

    # Disable everything we don't need (only build the library, not utilities)
    set(CONFIGURE_LIBLZMA_MODULES 
        --disable-doc 
        --disable-scripts 
        --disable-xz 
        --disable-xzdec 
        --disable-lzmadec 
        --disable-lzmainfo 
        --disable-lzma-links
    )

    # Additional configure script parameters
    # Enable both shared and static libraries
    set(CONFIGURE_LIBLZMA_PARAMS --enable-shared --enable-static)

    # Configure build commands based on compilation target
    if (CROSS)
        # Standard cross-compilation configuration
        set(COMMAND_CONFIGURE ../dist/configure ${CONFIGURE_LIBLZMA_PARAMS} --cross-compile-prefix=${CROSS_PREFIX} ${CROSS_TARGET} ${CONFIGURE_LIBLZMA_MODULES} --prefix=/usr/local/)
        set(COMMAND_TEST "true")
    elseif(CROSS_ANDROID)

        # Android specific configuration options
        # Note: Additional modules can be disabled here if needed
        # set(CONFIGURE_LIBLZMA_MODULES ${CONFIGURE_LIBLZMA_MODULES} no-hw)

        # Initialize compiler flags from CMake settings
        set(CFLAGS ${CMAKE_C_FLAGS})
        set(CXXFLAGS ${CMAKE_CXX_FLAGS})

        # Silence warnings about unused arguments (Clang specific)
        set(CFLAGS "${CMAKE_C_FLAGS} -Qunused-arguments")
        set(CXXFLAGS "${CMAKE_CXX_FLAGS} -Qunused-arguments")

        # Required environment configuration is already set (by e.g. ndk) so no need to fiddle around with all the options ...
        if (NOT ANDROID)
            message(FATAL_ERROR "Use NDK cmake toolchain or cmake android autoconfig")
        endif()

        if (ARMEABI_V7A)
            set(LIBLZMA_PLATFORM "--host armv7")
            #set(CONFIGURE_LIBLZMA_PARAMS ${CONFIGURE_LIBLZMA_PARAMS} "-march=armv7-a")
        else()
            if (CMAKE_ANDROID_ARCH_ABI MATCHES "arm64-v8a")
                set(LIBLZMA_PLATFORM "--host=aarch64-linux-android")
            else()
                set(LIBLZMA_PLATFORM "--host=${CMAKE_ANDROID_ARCH_ABI}")
            endif()
        endif()

        set(ANDROID_STRING "android")
        if (CMAKE_ANDROID_ARCH_ABI MATCHES "64")
            set(ANDROID_STRING "${ANDROID_STRING}64")
        endif()

        # copy over both sysroots to a common sysroot (workaround OpenSSL failing without one single sysroot)
        string(REPLACE "-clang" "" ANDROID_TOOLCHAIN_NAME ${ANDROID_TOOLCHAIN_NAME})
        file(COPY ${ANDROID_TOOLCHAIN_ROOT}/sysroot/usr/lib/${ANDROID_TOOLCHAIN_NAME}/${ANDROID_PLATFORM_LEVEL}/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/sysroot/usr/lib/)
        file(COPY ${ANDROID_TOOLCHAIN_ROOT}/sysroot/usr/lib/${ANDROID_TOOLCHAIN_NAME}/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/sysroot/usr/lib/ PATTERN *.*)
        file(COPY ${CMAKE_SYSROOT}/usr/include DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/sysroot/usr/)

        # Convert all CMake compiler options to environment variables
        # This is required because the configure script reads from environment
        set(CROSS_SYSROOT ${CMAKE_CURRENT_BINARY_DIR}/sysroot/)
        set(AS ${CMAKE_ASM_COMPILER})
        set(AR ${CMAKE_AR})
        set(LD ${CMAKE_LINKER})
        set(LDFLAGS ${CMAKE_MODULE_LINKER_FLAGS})

        # Have to surround variables with double quotes, otherwise they will be merged together without any separator
        # Construct compiler commands with all necessary flags and options
        set(CC "${CMAKE_C_COMPILER} ${CMAKE_C_COMPILE_OPTIONS_EXTERNAL_TOOLCHAIN}${CMAKE_C_COMPILER_EXTERNAL_TOOLCHAIN} ${CFLAGS} -target ${CMAKE_C_COMPILER_TARGET}")
        set(CXX "${CMAKE_CXX_COMPILER} ${CMAKE_CXX_COMPILE_OPTIONS_EXTERNAL_TOOLCHAIN}${CMAKE_CXX_COMPILER_EXTERNAL_TOOLCHAIN} ${CFLAGS} -target ${CMAKE_CXX_COMPILER_TARGET}")

        message(STATUS "AS:  ${AS}")
        message(STATUS "AR:  ${AR}")
        message(STATUS "LD:  ${LD}")
        message(STATUS "LDFLAGS: ${LDFLAGS}")
        message(STATUS "CC:  ${CC}")
        message(STATUS "CXX: ${CXX}")
        message(STATUS "ANDROID_TOOLCHAIN_ROOT: ${ANDROID_TOOLCHAIN_ROOT}")

        set(COMMAND_AUTOGEN ./autogen.sh)
        set(COMMAND_CONFIGURE ./configure --prefix=/usr/local/ ${CONFIGURE_LIBLZMA_PARAMS} ${LIBLZMA_PLATFORM} ${CONFIGURE_LIBLZMA_MODULES})
        set(COMMAND_TEST "true")
    else()                   # detect host system automatically
        set(COMMAND_AUTOGEN ./autogen.sh)
        set(COMMAND_CONFIGURE ./configure --prefix=/usr/local/ ${CONFIGURE_LIBLZMA_PARAMS} ${CONFIGURE_LIBLZMA_MODULES})
    endif()

    # Add ExternalProject target for building XZ library from source
    # This downloads, configures, builds, and installs XZ
    ExternalProject_Add(libxzExternal
            URL ${LIBXZ_ARCHIVE_LOCATION}/xz-${LIBXZ_BUILD_VERSION}.tar.gz
            ${LIBLZMA_CHECK_HASH}
            UPDATE_COMMAND ""
            # Run autogen.sh to generate configure script
            COMMAND ${COMMAND_AUTOGEN}
            # Configure the build
            CONFIGURE_COMMAND ${BUILD_ENV_TOOL} <SOURCE_DIR> ${COMMAND_CONFIGURE}
            # Build with parallel jobs
            BUILD_COMMAND ${BUILD_ENV_TOOL} <SOURCE_DIR>/${CONFIGURE_DIR} ${MAKE_PROGRAM} -j ${NUM_JOBS}
            BUILD_BYPRODUCTS ${LIBLZMA_PATH}
            # Install the built library
            INSTALL_COMMAND ${BUILD_ENV_TOOL} <SOURCE_DIR>/${CONFIGURE_DIR} ${PERL_PATH_FIX_INSTALL}
            COMMAND ${BUILD_ENV_TOOL} <SOURCE_DIR>/${CONFIGURE_DIR} ${MAKE_PROGRAM} DESTDIR=${CMAKE_CURRENT_BINARY_DIR} install
            # Force CMake reload to pick up the newly built library
            COMMAND ${CMAKE_COMMAND} -G ${CMAKE_GENERATOR} ${CMAKE_BINARY_DIR}
            # Copy installed files to parent directory for easier access
            COMMAND cp -r ${LIBLZMA_PREFIX}/usr/ ${LIBLZMA_PREFIX}/..

            # Enable logging for debugging
            LOG_CONFIGURE 1
            LOG_BUILD 1
            LOG_INSTALL 1
            )

    # set git config values to liblzma requirements (no impact on linux though)
    #    ExternalProject_Add_Step(liblzma setGitConfig
    #        COMMAND ${GIT_EXECUTABLE} config --global core.autocrlf false
    #        COMMAND ${GIT_EXECUTABLE} config --global core.eol lf
    #        DEPENDEES
    #        DEPENDERS download
    #        ALWAYS ON
    #    )

    # Set, don't abort if it fails (due to variables being empty). To realize this we must only call git if the configs
    # are set globally, otherwise do a no-op command ("echo 1", since "true" is not available everywhere)
    #    if (GIT_CORE_AUTOCRLF)
    #        set (GIT_CORE_AUTOCRLF_CMD ${GIT_EXECUTABLE} config --global core.autocrlf ${GIT_CORE_AUTOCRLF})
    #    else()
    #        set (GIT_CORE_AUTOCRLF_CMD echo)
    #    endif()
    #    if (GIT_CORE_EOL)
    #        set (GIT_CORE_EOL_CMD ${GIT_EXECUTABLE} config --global core.eol ${GIT_CORE_EOL})
    #    else()
    #        set (GIT_CORE_EOL_CMD echo)
    #    endif()
    ##

    # Set git config values to previous values
    #    ExternalProject_Add_Step(liblzma restoreGitConfig
    #        # Unset first (is required, since old value could be omitted, which wouldn't take any effect in "set"
    #        COMMAND ${GIT_EXECUTABLE} config --global --unset core.autocrlf
    #        COMMAND ${GIT_EXECUTABLE} config --global --unset core.eol
    #
    #        COMMAND ${GIT_CORE_AUTOCRLF_CMD}
    #        COMMAND ${GIT_CORE_EOL_CMD}
    #
    #        DEPENDEES download
    #        DEPENDERS configure
    #        ALWAYS ON
    #    )

    # Write environment variables to file for cross-compilation
    # This file is read by the Python building_env.py script to set up the build environment
    get_cmake_property(_variableNames VARIABLES)
    foreach (_variableName ${_variableNames})
        if (NOT _variableName MATCHES "lines")
            set(OUT_FILE "${OUT_FILE}${_variableName}=\"${${_variableName}}\"\n")
        endif()
    endforeach()
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/buildenv.txt ${OUT_FILE})

    # Set the imported library location property
    set_target_properties(lib_lzma PROPERTIES IMPORTED_LOCATION ${LIBLZMA_PATH})
endif()
