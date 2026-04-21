# SPDX-FileCopyrightText: © 2025 ALIAS Developers
# SPDX-FileCopyrightText: © 2020 Alias Developers
# SPDX-FileCopyrightText: © 2019 SpectreCoin Developers
# SPDX-License-Identifier: MIT
#
# Inspired by The ViaDuck Project for building OpenSSL
#
# BuildBerkeleyDB.cmake - CMake module for building BerkeleyDB from source
#
# This module handles the complete build process for BerkeleyDB, including:
# - Platform detection (Windows, Unix, Android cross-compilation)
# - Build tool detection (make, MSYS bash for Windows)
# - Configuration of build parameters for different platforms
# - External project setup for downloading and building BerkeleyDB
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
if (BERKELEYDB_ARCHIVE_HASH)
    set(BERKELEYDB_CHECK_HASH URL_HASH SHA256=${BERKELEYDB_ARCHIVE_HASH})
endif()

# Skip build if library already exists
if (EXISTS ${BERKELEYDB_LIBDB_PATH})
    message(STATUS "Not building BerkeleyDB again. Remove ${BERKELEYDB_LIBDB_PATH} for rebuild")
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
        # Note: CYGWIN and MSYS are handled implicitly as they provide Unix-like environment
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
#    execute_process(COMMAND ${GIT_EXECUTABLE} config --global --get core.autocrlf OUTPUT_VARIABLE GIT_CORE_AUTOCRLF OUTPUT_STRIP_TRAILING_WHITESPACE)
#    execute_process(COMMAND ${GIT_EXECUTABLE} config --global --get core.eol OUTPUT_VARIABLE GIT_CORE_EOL OUTPUT_STRIP_TRAILING_WHITESPACE)

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

    # Configure script parameters
    # Note: Additional modules can be disabled here if needed
    # set(CONFIGURE_BERKELEYDB_MODULES no-cast no-md2 no-md4 no-mdc2 no-rc4 no-rc5 no-engine no-idea no-mdc2 no-rc5 no-camellia no-ssl3 no-heartbeats no-gost no-deprecated no-capieng no-comp no-dtls no-psk no-srp no-dso no-dsa no-rc2 no-des)
    
    # Additional configure script parameters
    # Disable features we don't need and enable C++ support
    set(CONFIGURE_BERKELEYDB_PARAMS
            --disable-cryptography
            --disable-partition
            --disable-compression
            --disable-replication
            --enable-cxx
            --prefix=${CMAKE_INSTALL_PREFIX}
            --with-pic
            )

    # Configure build commands based on compilation target
    if (CROSS)
        # Standard cross-compilation configuration
        set(COMMAND_CONFIGURE ../dist/configure ${CONFIGURE_BERKELEYDB_PARAMS} --cross-compile-prefix=${CROSS_PREFIX} ${CROSS_TARGET} ${CONFIGURE_BERKELEYDB_MODULES})
        set(COMMAND_TEST "true")
    elseif(CROSS_ANDROID)
        
        # Android specific configuration options
        # Note: Additional modules can be disabled here if needed
        # set(CONFIGURE_BERKELEYDB_MODULES ${CONFIGURE_BERKELEYDB_MODULES} no-hw)

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
            set(BERKELEYDB_PLATFORM "--host armv7")
#            set(CONFIGURE_BERKELEYDB_PARAMS ${CONFIGURE_BERKELEYDB_PARAMS} "-march=armv7-a")
        else()
            if (CMAKE_ANDROID_ARCH_ABI MATCHES "arm64-v8a")
                set(BERKELEYDB_PLATFORM "--host arm")
            else()
                set(BERKELEYDB_PLATFORM "--host ${CMAKE_ANDROID_ARCH_ABI}")
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

        set(COMMAND_CONFIGURE ../dist/configure ${CONFIGURE_BERKELEYDB_PARAMS} ${BERKELEYDB_PLATFORM} ${CONFIGURE_BERKELEYDB_MODULES})
        set(COMMAND_TEST "true")
        set(CONFIGURE_DIR build_android)
    else()                   # detect host system automatically
        set(COMMAND_CONFIGURE ../dist/configure ${CONFIGURE_BERKELEYDB_PARAMS} ${CONFIGURE_BERKELEYDB_MODULES})
        set(CONFIGURE_DIR build_unix)
    endif()
    
    # Add ExternalProject target for building BerkeleyDB from source
    # This downloads, patches, configures, builds, and installs BerkeleyDB
    ExternalProject_Add(berkeleydb
        # Note: Alternative URLs can be used for testing
        # URL https://download.oracle.com/otn/berkeley-db/db-${BERKELEYDB_BUILD_VERSION}.tar.gz
        # URL file:///home/spectre/db-6.2.38.zip
        URL ${BERKELEYDB_ARCHIVE_LOCATION}/db-${BERKELEYDB_BUILD_VERSION}.tar.gz
        ${BERKELEYDB_CHECK_HASH}
        UPDATE_COMMAND ""
        
        # Apply patches for atomic operations and string literal fixes
        PATCH_COMMAND patch -p2 -d ${BERKELEYDB_PREFIX}/berkeleydb-prefix/src/berkeleydb < ${CMAKE_CURRENT_SOURCE_DIR}/patches/db-atomic.patch
        COMMAND patch -p1 -d ${BERKELEYDB_PREFIX}/berkeleydb-prefix/src/berkeleydb < ${CMAKE_CURRENT_SOURCE_DIR}/patches/fix-string-is-not-a-string-literal.patch

        # Update config.guess and config.sub as they're too old to detect aarch64
        # These files are used by autotools to detect the build system
        COMMAND rm -f ${BERKELEYDB_PREFIX}/berkeleydb-prefix/src/berkeleydb/dist/config.guess ${BERKELEYDB_PREFIX}/berkeleydb-prefix/src/berkeleydb/dist/config.sub
        COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/patches/config.guess ${BERKELEYDB_PREFIX}/berkeleydb-prefix/src/berkeleydb/dist/config.guess
        COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/patches/config.sub   ${BERKELEYDB_PREFIX}/berkeleydb-prefix/src/berkeleydb/dist/config.sub
        COMMAND chmod +x ${BERKELEYDB_PREFIX}/berkeleydb-prefix/src/berkeleydb/dist/config.guess ${BERKELEYDB_PREFIX}/berkeleydb-prefix/src/berkeleydb/dist/config.sub

        # Configure the build
        CONFIGURE_COMMAND ${BUILD_ENV_TOOL} <SOURCE_DIR>/${CONFIGURE_DIR} ${COMMAND_CONFIGURE}

        # Build with parallel jobs
        BUILD_COMMAND ${BUILD_ENV_TOOL} <SOURCE_DIR>/${CONFIGURE_DIR} ${MAKE_PROGRAM} -j ${NUM_JOBS}
        BUILD_BYPRODUCTS ${BERKELEYDB_LIBDB_PATH}

        # Install the built library
        INSTALL_COMMAND ${BUILD_ENV_TOOL} <SOURCE_DIR>/${CONFIGURE_DIR} ${PERL_PATH_FIX_INSTALL}
        COMMAND ${BUILD_ENV_TOOL} <SOURCE_DIR>/${CONFIGURE_DIR} ${MAKE_PROGRAM} DESTDIR=${CMAKE_CURRENT_BINARY_DIR} install
        # Force CMake reload to pick up the newly built library
        COMMAND ${CMAKE_COMMAND} -G ${CMAKE_GENERATOR} ${CMAKE_BINARY_DIR}

        # Enable logging for debugging
        LOG_INSTALL 1
        LOG_CONFIGURE 1
    )

    # set git config values to berkeleydb requirements (no impact on linux though)
#    ExternalProject_Add_Step(berkeleydb setGitConfig
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
#    ExternalProject_Add_Step(berkeleydb restoreGitConfig
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
    set_target_properties(bdb_lib PROPERTIES IMPORTED_LOCATION ${BERKELEYDB_LIBDB_PATH})
endif()
