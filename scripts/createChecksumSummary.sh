#!/bin/bash
# ===========================================================================
#
# SPDX-FileCopyrightText: © 2020 Alias Developers
# SPDX-FileCopyrightText: © 2016 SpectreCoin Developers
# SPDX-License-Identifier: MIT
#
# Created: 2019-01-22 HLXEasy
#
# Helper script to create checksums for given file (1st parameter)
# and write them into given another file (2nd parameter)
#
# ===========================================================================

releaseDescription=$1
workspace=$2
jobURL=$3
accessToken=$4

if test -e "${releaseDescription}" ; then
    cp "${releaseDescription}" "${workspace}"/releaseNotesToDeploy.txt
else
    echo "### ${releaseDescription}" > "${workspace}"/releaseNotesToDeploy.txt
fi
for currentChecksumfile in \
    Checksum-Alias-CentOS-8.txt \
    Checksum-Alias-Debian-Buster.txt \
    Checksum-Alias-Debian-Stretch.txt \
    Checksum-Alias-Fedora.txt \
    Checksum-Alias-OpenSUSE-Tumbleweed.txt \
    Checksum-Alias-Mac.txt \
    Checksum-Alias-Mac-OBFS4.txt \
    Checksum-Alias-RaspberryPi-Buster.txt \
    Checksum-Alias-RaspberryPi-Buster-aarch64.txt \
    Checksum-Alias-RaspberryPi-Stretch.txt \
    Checksum-Alias-Ubuntu-18-04.txt \
    Checksum-Alias-Ubuntu-20-04.txt \
    Checksum-Alias-Win64.txt \
    Checksum-Alias-Win64-OBFS4.txt \
    Checksum-Alias-Win64-Qt5.12.txt \
    Checksum-Alias-Win64-Qt5.12-OBFS4.txt \
    Checksum-Alias-Win64-Qt5.9.6.txt \
    Checksum-Alias-Win64-Qt5.9.6-OBFS4.txt ; do
    curl -X POST -L --user "${accessToken}" "${jobURL}"/artifact/${currentChecksumfile} --output ${currentChecksumfile} || true
    if [[ -e "${currentChecksumfile}" ]] && [[ $(wc -l < "${currentChecksumfile}") -eq 1 ]] ; then
        archiveFilename=$(cut -d ' ' -f1 ${currentChecksumfile})
        checksum=$(cut -d ' ' -f2 ${currentChecksumfile})
        echo "**${archiveFilename}:** \`${checksum}\`" >> "${workspace}"/releaseNotesToDeploy.txt
        echo '' >> "${workspace}"/releaseNotesToDeploy.txt
    fi
done
