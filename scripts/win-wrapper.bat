:: SPDX-FileCopyrightText: © 2025 ALIAS Developers
:: SPDX-FileCopyrightText: © 2020 Alias Developers
:: SPDX-FileCopyrightText: © 2016 SpectreCoin Developers
::
:: SPDX-License-Identifier: MIT
::
:: Wrapper script to define all requirements

@echo off
setlocal enabledelayedexpansion

:: Set version and Qt directory
set "ALIASWALLET_VERSION=4.2.0"
set "QTDIR=C:\Qt\5.15.0\msvc2019_64"

:: Generate build.h
call "scripts\win-genbuild.bat"
if errorlevel 1 (
    echo Failed to generate build.h
    endlocal
    exit /b %errorlevel%
)

:: Build the project
call "scripts\win-build.bat"
if errorlevel 1 (
    echo Failed to build project
    endlocal
    exit /b %errorlevel%
)

endlocal
exit /b 0
