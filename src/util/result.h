// SPDX-FileCopyrightText: © 2025 ALIAS Developers
// SPDX-FileCopyrightText: © 2020 Alias Developers
// Copyright (c) 2022 The Bitcoin Core developers
// SPDX-License-Identifier: MIT

#ifndef ALIAS_UTIL_RESULT_H
#define ALIAS_UTIL_RESULT_H

#include <optional>
#include <string>
#include <variant>
#include <utility>

namespace util {

//! Bilingual string for user-facing messages
struct bilingual_str {
    std::string original;
    std::string translated;

    bilingual_str() = default;
    bilingual_str(std::string original_in, std::string translated_in = "")
        : original(std::move(original_in))
        , translated(translated_in.empty() ? original : std::move(translated_in)) {}

    bool empty() const { return original.empty(); }
    void clear() { original.clear(); translated.clear(); }

    bilingual_str& operator+=(const bilingual_str& other) {
        original += other.original;
        translated += other.translated;
        return *this;
    }

    friend bilingual_str operator+(bilingual_str a, const bilingual_str& b) {
        a += b;
        return a;
    }
};

//! Simple Result type for error handling
//! Holds either a success value of type T, or an error message
template<typename T>
class Result {
public:
    Result() = default;

    Result(T value) : m_value(std::move(value)), m_has_value(true) {}

    static Result Error(std::string error) {
        Result r;
        r.m_error = std::move(error);
        r.m_has_value = false;
        return r;
    }

    static Result Error(bilingual_str error) {
        Result r;
        r.m_error = std::move(error.original);
        r.m_has_value = false;
        return r;
    }

    explicit operator bool() const { return m_has_value; }

    bool has_value() const { return m_has_value; }

    const T& value() const& { return m_value; }
    T& value() & { return m_value; }
    T&& value() && { return std::move(m_value); }

    const T* operator->() const { return &m_value; }
    T* operator->() { return &m_value; }

    const T& operator*() const& { return m_value; }
    T& operator*() & { return m_value; }
    T&& operator*() && { return std::move(m_value); }

    const std::string& error() const { return m_error; }

    template<typename U>
    T value_or(U&& default_value) const& {
        return m_has_value ? m_value : static_cast<T>(std::forward<U>(default_value));
    }

    template<typename U>
    T value_or(U&& default_value) && {
        return m_has_value ? std::move(m_value) : static_cast<T>(std::forward<U>(default_value));
    }

private:
    T m_value{};
    std::string m_error;
    bool m_has_value{false};
};

//! Specialization for void
template<>
class Result<void> {
public:
    Result() : m_has_value(true) {}

    static Result Error(std::string error) {
        Result r;
        r.m_error = std::move(error);
        r.m_has_value = false;
        return r;
    }

    static Result Error(bilingual_str error) {
        Result r;
        r.m_error = std::move(error.original);
        r.m_has_value = false;
        return r;
    }

    explicit operator bool() const { return m_has_value; }
    bool has_value() const { return m_has_value; }
    const std::string& error() const { return m_error; }

private:
    std::string m_error;
    bool m_has_value{false};
};

} // namespace util

//! Convenience function to create bilingual strings
inline util::bilingual_str _(const char* str) {
    return util::bilingual_str{str, str};
}

inline util::bilingual_str Untranslated(std::string original) {
    return util::bilingual_str{std::move(original)};
}

#endif // ALIAS_UTIL_RESULT_H
