// SPDX-FileCopyrightText: © 2025 ALIAS Developers
// SPDX-FileCopyrightText: © 2020 Alias Developers
// Copyright (c) 2009-2022 The Bitcoin Core developers
// SPDX-License-Identifier: MIT

#include <util/strencodings.h>

#include <algorithm>
#include <cstring>
#include <cassert>

namespace util {

static const char hexmap[16] = {'0', '1', '2', '3', '4', '5', '6', '7',
                                 '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

std::string HexStr(std::span<const uint8_t> bytes) {
    std::string rv;
    rv.reserve(bytes.size() * 2);
    for (uint8_t v : bytes) {
        rv.push_back(hexmap[v >> 4]);
        rv.push_back(hexmap[v & 15]);
    }
    return rv;
}

std::vector<uint8_t> ParseHex(std::string_view hex_str) {
    std::vector<uint8_t> rv;
    rv.reserve(hex_str.size() / 2);
    
    size_t pos = 0;
    while (pos < hex_str.size()) {
        while (pos < hex_str.size() && IsSpace(hex_str[pos])) {
            ++pos;
        }
        if (pos >= hex_str.size()) break;
        
        if (pos + 1 >= hex_str.size()) break;
        
        int hi = HexDigitValue(hex_str[pos]);
        int lo = HexDigitValue(hex_str[pos + 1]);
        if (hi < 0 || lo < 0) break;
        
        rv.push_back(static_cast<uint8_t>((hi << 4) | lo));
        pos += 2;
    }
    return rv;
}

bool IsHex(std::string_view str) {
    for (char c : str) {
        if (!IsHexDigit(c)) return false;
    }
    return (str.size() > 0) && (str.size() % 2 == 0);
}

std::string TrimString(std::string_view str, std::string_view pattern) {
    auto start = str.find_first_not_of(pattern);
    if (start == std::string_view::npos) return "";
    auto end = str.find_last_not_of(pattern);
    return std::string(str.substr(start, end - start + 1));
}

std::string_view TrimStringLeft(std::string_view str, std::string_view pattern) {
    auto start = str.find_first_not_of(pattern);
    if (start == std::string_view::npos) return "";
    return str.substr(start);
}

std::string_view TrimStringRight(std::string_view str, std::string_view pattern) {
    auto end = str.find_last_not_of(pattern);
    if (end == std::string_view::npos) return "";
    return str.substr(0, end + 1);
}

std::string ToLower(std::string_view str) {
    std::string result;
    result.reserve(str.size());
    for (char c : str) {
        result.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
    }
    return result;
}

std::string ToUpper(std::string_view str) {
    std::string result;
    result.reserve(str.size());
    for (char c : str) {
        result.push_back(static_cast<char>(std::toupper(static_cast<unsigned char>(c))));
    }
    return result;
}

bool EqualsIgnoreCase(std::string_view a, std::string_view b) {
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); ++i) {
        if (std::tolower(static_cast<unsigned char>(a[i])) != 
            std::tolower(static_cast<unsigned char>(b[i]))) {
            return false;
        }
    }
    return true;
}

std::vector<std::string> SplitString(std::string_view str, char sep) {
    std::vector<std::string> result;
    size_t start = 0;
    size_t end;
    while ((end = str.find(sep, start)) != std::string_view::npos) {
        result.emplace_back(str.substr(start, end - start));
        start = end + 1;
    }
    result.emplace_back(str.substr(start));
    return result;
}

std::vector<std::string> SplitString(std::string_view str, std::string_view sep) {
    std::vector<std::string> result;
    size_t start = 0;
    size_t end;
    while ((end = str.find(sep, start)) != std::string_view::npos) {
        result.emplace_back(str.substr(start, end - start));
        start = end + sep.size();
    }
    result.emplace_back(str.substr(start));
    return result;
}

std::optional<int64_t> ParseInt64(std::string_view str) {
    return ToIntegral<int64_t>(str);
}

std::optional<int32_t> ParseInt32(std::string_view str) {
    return ToIntegral<int32_t>(str);
}

std::optional<uint64_t> ParseUInt64(std::string_view str) {
    return ToIntegral<uint64_t>(str);
}

std::optional<uint32_t> ParseUInt32(std::string_view str) {
    return ToIntegral<uint32_t>(str);
}

std::optional<double> ParseDouble(std::string_view str) {
    try {
        size_t pos;
        double value = std::stod(std::string(str), &pos);
        if (pos == str.size()) return value;
    } catch (...) {}
    return std::nullopt;
}

std::string FormatBytes(uint64_t bytes) {
    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    int unit = 0;
    double size = static_cast<double>(bytes);
    while (size >= 1024.0 && unit < 4) {
        size /= 1024.0;
        ++unit;
    }
    char buf[64];
    if (unit == 0) {
        snprintf(buf, sizeof(buf), "%" PRIu64 " %s", bytes, units[unit]);
    } else {
        snprintf(buf, sizeof(buf), "%.2f %s", size, units[unit]);
    }
    return buf;
}

static const char* pbase64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

std::string EncodeBase64(std::span<const uint8_t> input) {
    std::string result;
    result.reserve((input.size() + 2) / 3 * 4);
    
    size_t i = 0;
    while (i < input.size()) {
        uint32_t n = static_cast<uint32_t>(input[i++]) << 16;
        if (i < input.size()) n |= static_cast<uint32_t>(input[i++]) << 8;
        if (i < input.size()) n |= input[i++];
        
        result.push_back(pbase64[(n >> 18) & 0x3f]);
        result.push_back(pbase64[(n >> 12) & 0x3f]);
        result.push_back((i > input.size() + 1) ? '=' : pbase64[(n >> 6) & 0x3f]);
        result.push_back((i > input.size()) ? '=' : pbase64[n & 0x3f]);
    }
    return result;
}

static const int8_t decode64_table[256] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, 62, -1, -1, -1, 63, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1,
    -1, -1, -1, -1, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1, -1, 26, 27, 28,
    29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
    49, 50, 51, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};

std::optional<std::vector<uint8_t>> DecodeBase64(std::string_view input) {
    std::vector<uint8_t> result;
    result.reserve(input.size() * 3 / 4);
    
    uint32_t n = 0;
    int bits = 0;
    
    for (char c : input) {
        if (c == '=') break;
        if (IsSpace(c)) continue;
        
        int8_t val = decode64_table[static_cast<uint8_t>(c)];
        if (val < 0) return std::nullopt;
        
        n = (n << 6) | val;
        bits += 6;
        
        if (bits >= 8) {
            bits -= 8;
            result.push_back(static_cast<uint8_t>((n >> bits) & 0xff));
        }
    }
    return result;
}

std::string SanitizeString(std::string_view str, int rule) {
    std::string result;
    result.reserve(str.size());
    for (char c : str) {
        if (c >= 32 && c <= 126) {
            result.push_back(c);
        }
    }
    return result;
}

std::string UrlEncode(std::string_view str) {
    std::string result;
    result.reserve(str.size() * 3);
    for (unsigned char c : str) {
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
            (c >= '0' && c <= '9') || c == '-' || c == '_' || c == '.' || c == '~') {
            result.push_back(c);
        } else {
            result.push_back('%');
            result.push_back(hexmap[c >> 4]);
            result.push_back(hexmap[c & 15]);
        }
    }
    return result;
}

std::string UrlDecode(std::string_view str) {
    std::string result;
    result.reserve(str.size());
    for (size_t i = 0; i < str.size(); ++i) {
        if (str[i] == '%' && i + 2 < str.size()) {
            int hi = HexDigitValue(str[i + 1]);
            int lo = HexDigitValue(str[i + 2]);
            if (hi >= 0 && lo >= 0) {
                result.push_back(static_cast<char>((hi << 4) | lo));
                i += 2;
                continue;
            }
        }
        result.push_back(str[i]);
    }
    return result;
}

} // namespace util
