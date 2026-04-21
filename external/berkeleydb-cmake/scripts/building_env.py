# SPDX-FileCopyrightText: © 2025 ALIAS Developers
# SPDX-FileCopyrightText: © 2020 Alias Developers
# SPDX-FileCopyrightText: © 2019 SpectreCoin Developers
# SPDX-License-Identifier: MIT
#
# Inspired by The ViaDuck Project for building OpenSSL
#
# building_env.py - Build environment setup script for BerkeleyDB
#
# Creates a building environment for BerkeleyDB:
# - Sets working directory
# - On Windows: uses MSYS bash for command execution (BerkeleyDB's scripts need an UNIX-like environment with perl)
# - Handles cross-compilation environment setup for Android

from subprocess import PIPE, Popen
from sys import argv
import os
import re

env = os.environ
l = []

os_s = argv[1]                                      # operating system
offset = 2          # 0: this script's path, 1: operating system

if os_s == "WIN32":
    offset = 4  # 2: MSYS_BASH_PATH, 3: CMAKE_MAKE_PROGRAM

    #
    bash = argv[2]
    msys_path = os.path.dirname(bash)
    mingw_path = os.path.dirname(argv[3])

    # append ; to PATH if needed
    if not env['PATH'].endswith(";"):
        env['PATH'] += ";"

    # include path of msys binaries (perl, cd etc.) and building tools (gcc, ld etc.)
    env['PATH'] = ";".join([msys_path, mingw_path])+";"+env['PATH']
    env['MAKEFLAGS'] = ''            # otherwise: internal error: invalid --jobserver-fds string `gmake_semaphore_1824'


binary_berkeleydb_dir_source = argv[offset]+"/"             # downloaded berkeleydb source dir
l.extend(argv[offset+1:])                             # routed commands

l[0] = '"'+l[0]+'"'

# ensure target dir exists for mingw cross
target_dir = binary_berkeleydb_dir_source+"/../../../usr/local/bin"
if not os.path.exists(target_dir):
    os.makedirs(target_dir)

# Read environment from file if cross-compiling for Android
if os_s == "LINUX_CROSS_ANDROID":
    expr = re.compile(r'^(.*?)="(.*?)"', re.MULTILINE | re.DOTALL)
    # Note: Using string concatenation for relative paths with ../
    env_file_path = binary_berkeleydb_dir_source + "/../../../../buildenv.txt"
    
    # Use modern Python file handling with context manager
    with open(env_file_path, "r", encoding='utf-8') as f:
        content = f.read()

    for k, v in expr.findall(content):
        if "\n" in k.strip():
            print('Skipping multiline key')
        elif k != "PATH":
            env[k] = v.replace('"', '')
        else:
            # Prepend to PATH to ensure cross-compilation tools are found first
            env[k] = v.replace('"', '') + ":" + env[k]

# Execute the build command in the appropriate environment
proc = None
if os_s == "WIN32":
    # We must emulate a UNIX environment to build BerkeleyDB using MinGW/MSYS
    proc = Popen(bash, env=env, cwd=binary_berkeleydb_dir_source, stdin=PIPE, universal_newlines=True)
    proc.communicate(input=" ".join(l) + " || exit $?")
else:
    # On Unix-like systems, execute directly with shell
    proc = Popen(" ".join(l) + " || exit $?", shell=True, env=env, cwd=binary_berkeleydb_dir_source)
    proc.communicate()

# Exit with the return code from the build command
exit(proc.returncode)
