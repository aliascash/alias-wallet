# SPDX-FileCopyrightText: © 2025 ALIAS Developers
# SPDX-FileCopyrightText: © 2020 Alias Developers
# SPDX-FileCopyrightText: © 2019 SpectreCoin Developers
# SPDX-License-Identifier: MIT
#
# DownloadFile.cmake
# ===================
# Modern CMake function to download a file with hash verification.
#
# Usage:
#   download_file(<url> <filename> <hash_type> <hash>)
#
# Parameters:
#   url        - URL of the file to download
#   filename   - Local path where the file will be saved
#   hash_type  - Hash algorithm (e.g., SHA256, MD5)
#   hash       - Expected hash value for verification
#
# The function will skip downloading if the file already exists.
# Uses TLS verification for secure downloads.

function(download_file url filename hash_type hash)
    if(NOT EXISTS "${filename}")
        file(DOWNLOAD "${url}" "${filename}"
                TIMEOUT 60  # seconds
                EXPECTED_HASH ${hash_type}=${hash}
                TLS_VERIFY ON)
    endif()
endfunction()
