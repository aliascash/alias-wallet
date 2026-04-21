:: ===========================================================================
::
:: SPDX-FileCopyrightText: © 2025 ALIAS Developers
:: SPDX-FileCopyrightText: © 2020 Alias Developers
:: SPDX-FileCopyrightText: © 2019 SpectreCoin Developers
:: SPDX-License-Identifier: MIT
::
:: Created: 2019-10-22 HLXEasy
::
:: Helper script to build Alias on Windows using VS2019 and QT.
::
:: ===========================================================================

@echo off
setlocal enabledelayedexpansion

:: Check for required environment variables
IF "%QTDIR_x86%" == "" GOTO NOQT
:YESQT

IF "%VSDIR%" == "" GOTO NOVS
:YESVS

IF "%CMAKEDIR_x86%" == "" GOTO NOCMAKE
:YESVS

IF "%VCPKGDIR%" == "" GOTO NOVCPKG
:YESVS

:: Set up directory paths
set "SRC_DIR=%cd%"
set "BUILD_DIR=%cd%\build-x86"

:: Initialize Visual Studio environment (32-bit)
:: "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"
call "%VSDIR%\VC\Auxiliary\Build\vcvars32.bat"
if errorlevel 1 (
    echo Failed to initialize Visual Studio environment
    endlocal
    exit /b 1
)

cd
cd "%SRC_DIR%"
if errorlevel 1 (
    echo Failed to change to source directory: %SRC_DIR%
    endlocal
    exit /b 1
)
dir

echo on

:: Clean and prepare build directory
if exist "%BUILD_DIR%\delivery" (
    rmdir /S /Q "%BUILD_DIR%\delivery"
    if errorlevel 1 (
        echo Warning: Failed to remove delivery directory
    )
)
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
if errorlevel 1 goto :ERROR

cd "%BUILD_DIR%"
if errorlevel 1 goto :ERROR

:: Configure CMake for 32-bit build
"%CMAKEDIR_x86%\cmake.exe" -D CMAKE_TOOLCHAIN_FILE="%VCPKGDIR%\scripts\buildsystems\vcpkg.cmake" -D CMAKE_FIND_ROOT_PATH_MODE_LIBRARY=NEVER -D CMAKE_FIND_ROOT_PATH_MODE_INCLUDE=NEVER -D ENABLE_GUI=ON -D QT_CMAKE_MODULE_PATH="%QTDIR_x86%\lib\cmake" -D CMAKE_BUILD_TYPE=Release -G "Visual Studio 16 2019" -A Win32 ..
if errorlevel 1 goto :ERROR

:: Build the project
"%CMAKEDIR_x86%\cmake.exe" --build . --target Aliaswallet --config Release
if errorlevel 1 goto :ERROR

::ren "%OUT_DIR%" Alias
::echo "The prepared package is in: %BUILD_DIR%\delivery"

echo "Everything is OK"
GOTO END

:ERROR
echo Failed with error #%errorlevel%.
cd "%SRC_DIR%"
endlocal
exit /b %errorlevel%
GOTO END

:NOVCPKG
@ECHO The VCPKGDIR environment variable was NOT detected!
endlocal
exit /b 1
GOTO END

:NOCMAKE
@ECHO The CMAKEDIR_x86 environment variable was NOT detected!
endlocal
exit /b 1
GOTO END

:NOVS
@ECHO The VSDIR environment variable was NOT detected!
endlocal
exit /b 1
GOTO END

:NOQT
@ECHO The QTDIR_x86 environment variable was NOT detected!
endlocal
exit /b 1

:END
cd "%SRC_DIR%"
endlocal
