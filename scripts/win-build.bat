:: SPDX-FileCopyrightText: © 2025 ALIAS Developers
:: SPDX-FileCopyrightText: © 2020 Alias Developers
:: SPDX-FileCopyrightText: © 2016 SpectreCoin Developers
::
:: SPDX-License-Identifier: MIT
::
:: Helper script to build Aliaswallet on Windows using VS2017 and QT.

@echo off
setlocal enabledelayedexpansion

:: Check for required environment variables
IF "%QTDIR%" == "" GOTO NOQT
:YESQT

IF "%VSDIR%" == "" GOTO NOVS
:YESVS

:: Set up directory paths
set "CALL_DIR=%cd%"
set "SRC_DIR=%cd%\src"
set "DIST_DIR=%SRC_DIR%\dist"
set "BUILD_DIR=%SRC_DIR%\build"
set "OUT_DIR=%SRC_DIR%\bin"

:: Initialize Visual Studio environment
:: "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"
call "%VSDIR%\Community\VC\Auxiliary\Build\vcvars64.bat"
if errorlevel 1 (
    echo Failed to initialize Visual Studio environment
    exit /b 1
)

cd
cd "%SRC_DIR%"
if errorlevel 1 (
    echo Failed to change to source directory: %SRC_DIR%
    exit /b 1
)
dir

echo on

:: Clean previous build artifacts
del "%OUT_DIR%\Alias.exe" 2>nul
if exist "%DIST_DIR%" (
    rmdir /S /Q "%DIST_DIR%"
    if errorlevel 1 (
        echo Warning: Failed to remove dist directory
    )
)
if not exist "%DIST_DIR%" mkdir "%DIST_DIR%"
if errorlevel 1 goto :ERROR

if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
if errorlevel 1 goto :ERROR

if not exist "%OUT_DIR%" mkdir "%OUT_DIR%"
if errorlevel 1 goto :ERROR

:: Build using qmake
pushd "%BUILD_DIR%"

"%QTDIR%\bin\qmake.exe" ^
  -spec win32-msvc ^
  "CONFIG += release" ^
  "%SRC_DIR%\src.pro"
if errorlevel 1 goto :ERROR

nmake
if errorlevel 1 goto :ERROR

popd

:: Deploy Qt dependencies
"%QTDIR%\bin\windeployqt.exe" --force --qmldir "%SRC_DIR%\qt\res" --qml --quick --webengine "%OUT_DIR%\Alias.exe"
if errorlevel 1 goto :ERROR

::ren "%OUT_DIR%" Alias
::echo "The prepared package is in: %SRC_DIR%\Alias"

echo "Everything is OK"
GOTO END

:ERROR
echo Failed with error #%errorlevel%.
endlocal
exit /b %errorlevel%
GOTO END

:NOVS
@ECHO The VSDIR environment variable was NOT detected!
endlocal
exit /b 1
GOTO END

:NOQT
@ECHO The QTDIR environment variable was NOT detected!
endlocal
exit /b 1

:END
cd "%CALL_DIR%"
endlocal
