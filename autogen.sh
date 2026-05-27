#!/bin/bash -e
# ===========================================================================
#
# SPDX-FileCopyrightText: © 2020 Alias Developers
# SPDX-FileCopyrightText: © 2016 SpectreCoin Developers
# SPDX-License-Identifier: MIT
#
# ===========================================================================
#
# Generates the autotools build files (./configure + Makefile.in's) from
# configure.ac and src/Makefile.am. Required once after a fresh checkout
# and after any change to configure.ac or *.am files.
#
# Run from the repository root:
#   ./autogen.sh
#   ./configure --enable-gui
#   make
#
# The legacy CMake build under scripts/cmake-build*.{sh,bat} is being
# retired in favor of this autotools flow.

ownLocation="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd "${ownLocation}"

autoreconf --install --force --warnings=all
