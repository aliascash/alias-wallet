// SPDX-FileCopyrightText: © 2025 ALIAS Developers
// SPDX-FileCopyrightText: © 2020 Alias Developers
// Copyright (c) 2009-2022 The Bitcoin Core developers
// SPDX-License-Identifier: MIT

#ifndef ALIAS_UTIL_STRENCODINGS_H
#define ALIAS_UTIL_STRENCODINGS_H

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include <charconv>
#include <span>

namespace util {

//! Check if character is a space
constexpr inline bool IsSpace(char c) noexcept {
    return c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v';
}

//! Check if character is a digit
constexpr inline bool IsDigit(char c) {
    return c >= '0' && c <= '9';
}

//! Check if character is a hex digit
constexpr inline bool IsHexDigit(char c) {
    return IsDigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

//! Convert hex character to integer
constexpr inline int HexDigitValue(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

//! Convert bytes to hex string
std::string HexStr(std::span<const uint8_t> bytes);

inline std::string HexStr(std::span<const char> bytes) {
    return HexStr(std::span<const uint8_t>(
        reinterpret_cast<const uint8_t*>(bytes.data()), bytes.size()));
}

//! Convert hex string to bytes
std::vector<uint8_t> ParseHex(std::string_view hex_str);

//! Check if string is valid hex
bool IsHex(std::string_view str);

//! Trim whitespace from string
std::string TrimString(std::string_view str, std::string_view pattern = " \f\n\r\t\v");

//! Left trim whitespace
std::string_view TrimStringLeft(std::string_view str, std::string_view pattern = " \f\n\r\t\v");

//! Right trim whitespace
std::string_view TrimStringRight(std::string_view str, std::string_view pattern = " \f\n\r\t\v");

//! Convert string to lowercase
std::string ToLower(std::string_view str);

//! Convert string to uppercase
std::string ToUpper(std::string_view str);

//! Check if strings are equal (case-insensitive)
bool EqualsIgnoreCase(std::string_view a, std::string_view b);

//! Join strings with separator
template<typename Container>
std::string Join(const Container& container, std::string_view separator) {
    std::string result;
    bool first = true;
    for (const auto& item : container) {
        if (!first) result += separator;
        result += item;
        first = false;
    }
    return result;
}

//! Split string by separator
std::vector<std::string> SplitString(std::string_view str, char sep);
std::vector<std::string> SplitString(std::string_view str, std::string_view sep);

//! Parse integer from string
template<typename T>
std::optional<T> ToIntegral(std::string_view str) {
    T value;
    const auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), value);
    if (ec == std::errc{} && ptr == str.data() + str.size()) {
        return value;
    }
    return std::nullopt;
}

//! Safe conversion from string to int64
std::optional<int64_t> ParseInt64(std::string_view str);

//! Safe conversion from string to int32
std::optional<int32_t> ParseInt32(std::string_view str);

//! Safe conversion from string to uint64
std::optional<uint64_t> ParseUInt64(std::string_view str);

//! Safe conversion from string to uint32
std::optional<uint32_t> ParseUInt32(std::string_view str);

//! Safe conversion from string to double
std::optional<double> ParseDouble(std::string_view str);

//! Convert bytes to human-readable string
std::string FormatBytes(uint64_t bytes);

//! Encode bytes as base64
std::string EncodeBase64(std::span<const uint8_t> input);

//! Decode base64 to bytes
std::optional<std::vector<uint8_t>> DecodeBase64(std::string_view input);

//! Encode bytes as base32
std::string EncodeBase32(std::span<const uint8_t> input);

//! Decode base32 to bytes
std::optional<std::vector<uint8_t>> DecodeBase32(std::string_view input);

//! URL encode string
std::string UrlEncode(std::string_view str);

//! URL decode string
std::string UrlDecode(std::string_view str);

//! Sanitize string for safe display
std::string SanitizeString(std::string_view str, int rule = 0);

} // namespace util

#endif // ALIAS_UTIL_STRENCODINGS_H
