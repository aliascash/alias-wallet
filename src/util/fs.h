// SPDX-FileCopyrightText: © 2025 ALIAS Developers
// SPDX-FileCopyrightText: © 2020 Alias Developers
// Copyright (c) 2021-2022 The Bitcoin Core developers
// SPDX-License-Identifier: MIT

#ifndef ALIAS_UTIL_FS_H
#define ALIAS_UTIL_FS_H

#include <filesystem>
#include <fstream>
#include <string>
#include <optional>

namespace fs {

using namespace std::filesystem;

//! Path separator
#ifdef WIN32
static constexpr char path_separator = '\\';
#else
static constexpr char path_separator = '/';
#endif

//! Convert path to UTF-8 string
inline std::string PathToString(const path& p) {
#ifdef WIN32
    return p.u8string();
#else
    return p.string();
#endif
}

//! Convert UTF-8 string to path
inline path PathFromString(const std::string& s) {
#ifdef WIN32
    return path(std::u8string(s.begin(), s.end()));
#else
    return path(s);
#endif
}

//! Create directory if it doesn't exist
inline bool create_directories_safe(const path& p) {
    std::error_code ec;
    create_directories(p, ec);
    return !ec;
}

//! Check if path exists
inline bool exists_safe(const path& p) {
    std::error_code ec;
    return exists(p, ec);
}

//! Remove file
inline bool remove_safe(const path& p) {
    std::error_code ec;
    return remove(p, ec);
}

//! Get file size
inline std::optional<uintmax_t> file_size_safe(const path& p) {
    std::error_code ec;
    auto size = file_size(p, ec);
    if (ec) return std::nullopt;
    return size;
}

//! File input stream that works with std::filesystem::path
class ifstream : public std::ifstream {
public:
    ifstream() = default;
    explicit ifstream(const path& p, std::ios_base::openmode mode = std::ios_base::in)
        : std::ifstream(p, mode) {}
    void open(const path& p, std::ios_base::openmode mode = std::ios_base::in) {
        std::ifstream::open(p, mode);
    }
};

//! File output stream that works with std::filesystem::path
class ofstream : public std::ofstream {
public:
    ofstream() = default;
    explicit ofstream(const path& p, std::ios_base::openmode mode = std::ios_base::out)
        : std::ofstream(p, mode) {}
    void open(const path& p, std::ios_base::openmode mode = std::ios_base::out) {
        std::ofstream::open(p, mode);
    }
};

//! File stream that works with std::filesystem::path
class fstream : public std::fstream {
public:
    fstream() = default;
    explicit fstream(const path& p, std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out)
        : std::fstream(p, mode) {}
    void open(const path& p, std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out) {
        std::fstream::open(p, mode);
    }
};

} // namespace fs

#endif // ALIAS_UTIL_FS_H
