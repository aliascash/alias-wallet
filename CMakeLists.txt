# SPDX-FileCopyrightText: © 2025 ALIAS Developers
# SPDX-FileCopyrightText: © 2020 Alias Developers
# SPDX-FileCopyrightText: © 2019 SpectreCoin Developers
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.13)
project(ALIAS VERSION 5.1.0 LANGUAGES CXX C)

message(STATUS "")
message(STATUS "=== Setting defaults =======================================================")
set(CLIENT_VERSION_MAJOR 5)
set(CLIENT_VERSION_MINOR 1)
set(CLIENT_VERSION_REVISION 0)
set(CLIENT_VERSION_BUILD 0)
set(CLIENT_VERSION_GIT_SHA1 "---")
set(VERSION_STRING "${CLIENT_VERSION_MAJOR}.${CLIENT_VERSION_MINOR}.${CLIENT_VERSION_REVISION}.${CLIENT_VERSION_BUILD}")
set(CMAKE_VERBOSE_MAKEFILE OFF)
set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/android")

set(TOR_ARCHIVE_MAC_HASH "733380c234c8a4796d6bfefec536afd6469f8d7d")
set(TOR_ARCHIVE_WIN64_HASH "36ab45ca414795560a4c946fcb49848b919569bf")
set(TOR_ARCHIVE_WIN32_HASH "66cef46b02d6e788840899baca11e2a2f9f5241a")
set(VCRUNTIME_ARCHIVE_WIN64_HASH "85cffbf41e957dcc9c5dd528af84c03474e76b8c")
set(VCRUNTIME_ARCHIVE_WIN32_HASH "9af511350e0772bf1f543abd375304af2e616273")

set(CMAKE_CXX_STANDARD 17)
if (WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
    add_compile_definitions(NOMINMAX)
endif ()

if (ANDROID)
    set(CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES};DebugAndroid;ReleaseAndroid" CACHE STRING "" FORCE)
    if(DEFINED ENV{BOOST_ROOT})
        message(STATUS "Getting settings from environment")
        set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY NEVER)
        set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE NEVER)
        set(BOOST_ROOT "$ENV{BOOST_ROOT}")
        set(BOOST_INCLUDEDIR "$ENV{BOOST_INCLUDEDIR}")
        set(Boost_INCLUDE_DIR "$ENV{BOOST_INCLUDEDIR}")
        set(BOOST_LIBRARYDIR "$ENV{BOOST_LIBRARYDIR}")
        set(Boost_LIBRARY_DIR "$ENV{BOOST_LIBRARYDIR}")
        set(BerkeleyDB_ROOT_DIR "$ENV{BerkeleyDB_ROOT_DIR}")
        set(BERKELEYDB_INCLUDE_DIR "$ENV{BERKELEYDB_INCLUDE_DIR}")
        set(leveldb_DIR "$ENV{leveldb_DIR}")
        set(OPENSSL_ROOT_DIR "$ENV{OPENSSL_ROOT_DIR}")
        set(ENABLE_GUI ON)
    endif()
    message(STATUS "Configuring for Android")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wno-deprecated-declarations -Wno-dangling-else")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=gold")
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pie")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(STATUS "Configuring on/for Linux")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    message(STATUS "Configuring on/for macOS")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    message(STATUS "Configuring on/for Windows")
    add_definitions(-DBOOST_ALL_DYN_LINK)
    add_definitions("/wd4244")
    add_definitions("/wd4267")
    add_definitions(-DWIN32_LEAN_AND_MEAN)

    if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
        set(DEPLOY_QT_BINARY_TYPE_OPTION "--debug")
        set(WINDEPLOYQT_TARGET_FOLDER "Debug")
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/delivery/${WINDEPLOYQT_TARGET_FOLDER}")
    else()
        set(DEPLOY_QT_BINARY_TYPE_OPTION "--release")
        set(WINDEPLOYQT_TARGET_FOLDER "Release")
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/delivery/${WINDEPLOYQT_TARGET_FOLDER}")
    endif()

    find_program(UNZIP unzip)
    if (${UNZIP} STREQUAL "UNZIP-NOTFOUND")
        message(FATAL_ERROR "You need to install 'unzip'!")
    endif ()
endif ()

if (POLICY CMP0074)
    cmake_policy(SET CMP0074 NEW)
endif ()

message(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
include(GetGitRevisionDescription)
get_git_head_revision(GIT_REFSPEC CLIENT_VERSION_GIT_SHA1)
include(GNUInstallDirs)
message(STATUS "Building in '${CMAKE_BUILD_TYPE}' mode")

message(STATUS "")
message(STATUS "=== Boost ==================================================================")
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_STATIC_RUNTIME ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_NO_SYSTEM_PATHS ON)
set(Boost_ARCHITECTURE -x64)
find_package(Boost 1.72.0 REQUIRED COMPONENTS atomic chrono date_time filesystem iostreams program_options regex system thread)
message(STATUS "Boost_VERSION:      ${Boost_VERSION}")
message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
message(STATUS "Boost_LIBRARY_DIRS: ${Boost_LIBRARY_DIRS}")
message(STATUS "Boost_LIBRARIES:    ${Boost_LIBRARIES}")
add_compile_definitions(BOOST_SPIRIT_THREADSAFE)

message(STATUS "")
message(STATUS "=== Qt =====================================================================")
if (ENABLE_GUI)
    if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
        set(Qt5_DIR "C:/Qt/5.12.7/msvc2017_64/lib/cmake/Qt5")
        list(APPEND CMAKE_PREFIX_PATH "C:/Qt/5.12.7/msvc2017_64/lib/cmake")
    endif()

    set(CMAKE_INCLUDE_CURRENT_DIR ON)
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTOUIC ON)
    set(CMAKE_AUTORCC ON)

    find_package(Qt5 REQUIRED COMPONENTS
        Core
        Gui
        Widgets
        WebView
        WebChannel
        WebSockets
        QuickWidgets
        Quick
        Svg
        Concurrent
        LinguistTools
    )

    message(STATUS "Qt5Core version: ${Qt5Core_VERSION}")
    message(STATUS "Qt5Widgets version: ${Qt5Widgets_VERSION}")
    message(STATUS "Qt5Gui version: ${Qt5Gui_VERSION}")

    include("${Qt5_LinguistTools_DIR}/Qt5LinguistToolsMacros.cmake" OPTIONAL)

    if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
        get_target_property(_qmake_executable Qt5::qmake IMPORTED_LOCATION)
        get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
        find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/delivery)
    elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
        get_target_property(_qmake_executable Qt5::qmake IMPORTED_LOCATION)
        get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
        find_program(MACDEPLOYQT_EXECUTABLE macdeployqt HINTS "${_qt_bin_dir}")
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
    endif()
else()
    message(STATUS "Building without UI, skipping Qt detection")
endif()

message(STATUS "")
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    message(STATUS "=== Tor binary archive =====================================================")
    if (NOT TOR_ARCHIVE)
        include(DownloadFile)
        set(TOR_ARCHIVE "${CMAKE_BINARY_DIR}/Tor.libraries.MacOS.zip")
        download_file(https://github.com/aliascash/resources/raw/master/resources/Tor.libraries.MacOS.zip ${TOR_ARCHIVE} SHA1 ${TOR_ARCHIVE_MAC_HASH})
    endif ()
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    message(STATUS "=== Tor and VCRuntime binary archives ======================================")
    if (CMAKE_CL_64)
        message(STATUS "Using x64 archives")
        set(WINDOWS_ARCH_SUFFIX "Win64")
    else()
        message(STATUS "Using x86 archives")
        set(WINDOWS_ARCH_SUFFIX "Win32")
    endif()

    if (NOT TOR_ARCHIVE)
        message(STATUS "Downloading Tor archive (official Tor Project)")
        include(DownloadFile)
        set(TOR_ARCHIVE "${CMAKE_BINARY_DIR}/Tor.libraries.${WINDOWS_ARCH_SUFFIX}.zip")
        download_file(
            "https://archive.torproject.org/tor-package-archive/torbrowser/14.0.3/tor-win64-0.4.8.11.zip"
            ${TOR_ARCHIVE}
            SHA256 ""
        )
    else()
        message(STATUS "Using Tor archive from given path")
    endif()

    if (NOT VCRUNTIME_ARCHIVE)
        message(STATUS "Downloading VCRuntime archive (Microsoft official)")
        include(DownloadFile)
        set(VCRUNTIME_ARCHIVE "${CMAKE_BINARY_DIR}/VCRuntime.libraries.${WINDOWS_ARCH_SUFFIX}.zip")
        download_file(
            "https://aka.ms/vs/16/release/vc_redist.x64.exe"
            ${VCRUNTIME_ARCHIVE}
            SHA256 ""
        )
    else()
        message(STATUS "Using VCRuntime archive from given path")
    endif()
endif()

message(STATUS "")
message(STATUS "=== OpenSSL ================================================================")
set(OPENSSL_USE_STATIC_LIBS TRUE)
find_package(OpenSSL)
message(STATUS "OPENSSL_FOUND: ${OPENSSL_FOUND}")
message(STATUS "OPENSSL_INCLUDE_DIR: ${OPENSSL_INCLUDE_DIR}")
message(STATUS "OPENSSL_CRYPTO_LIBRARY: ${OPENSSL_CRYPTO_LIBRARY}")
message(STATUS "OPENSSL_SSL_LIBRARY: ${OPENSSL_SSL_LIBRARY}")
message(STATUS "OPENSSL_LIBRARIES: ${OPENSSL_LIBRARIES}")
message(STATUS "OPENSSL_VERSION: ${OPENSSL_VERSION}")

message(STATUS "")
message(STATUS "=== Berkeley DB ============================================================")
find_package(BerkeleyDB 4)

message(STATUS "")
message(STATUS "=== LevelDB ================================================================")
find_package(leveldb)
message(STATUS "LevelDB directory: ${leveldb_DIR}")
message(STATUS "LevelDB version: ${leveldb_VERSION}")

message(STATUS "")
message(STATUS "=== Dive into subdirs... ===================================================")
add_subdirectory(src)
