# SPDX-FileCopyrightText: © 2020 Alias Developers
# SPDX-FileCopyrightText: © 2019 SpectreCoin Developers
# SPDX-License-Identifier: MIT

function(download_file url filename hash_type hash)
    if (NOT EXISTS ${filename})
        file(DOWNLOAD ${url} ${filename}
                TIMEOUT 60  # seconds
                EXPECTED_HASH ${hash_type}=${hash}
                TLS_VERIFY ON
                STATUS downloadStatus)
        list(GET downloadStatus 0 downloadCode)
        if (NOT downloadCode EQUAL 0)
            list(GET downloadStatus 1 downloadError)
            # Remove the partial file, otherwise the next configure run sees it
            # and skips the download.
            file(REMOVE ${filename})
            message(FATAL_ERROR "Download failed: ${url}\n  ${downloadError}")
        endif ()
    endif ()
endfunction(download_file)
