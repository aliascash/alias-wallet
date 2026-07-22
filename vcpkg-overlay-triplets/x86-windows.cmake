# Overlay of vcpkg's built-in x86-windows triplet.
# The pinned 2021 vcpkg's bzip2 port runs vcpkg_fixup_pkgconfig, which tries to
# download an msys2 pkg-config package that msys2 has removed from every mirror.
# Point PKGCONFIG at the runner-installed pkg-config so no msys download happens.
set(VCPKG_TARGET_ARCHITECTURE x86)
set(VCPKG_CRT_LINKAGE dynamic)
set(VCPKG_LIBRARY_LINKAGE dynamic)

set(PKGCONFIG "C:/ProgramData/chocolatey/bin/pkg-config.exe")
