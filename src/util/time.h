// SPDX-FileCopyrightText: © 2025 ALIAS Developers
// SPDX-FileCopyrightText: © 2020 Alias Developers
// Copyright (c) 2009-2022 The Bitcoin Core developers
// SPDX-License-Identifier: MIT

#ifndef ALIAS_UTIL_TIME_H
#define ALIAS_UTIL_TIME_H

#include <chrono>
#include <cstdint>
#include <string>
#include <thread>

using namespace std::chrono_literals;

namespace util {

//! Get current system time in seconds since epoch
inline int64_t GetTime() {
    return std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
}

//! Get current system time in milliseconds since epoch
inline int64_t GetTimeMillis() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
}

//! Get current system time in microseconds since epoch
inline int64_t GetTimeMicros() {
    return std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
}

//! Sleep for specified milliseconds
inline void MilliSleep(int64_t n) {
    std::this_thread::sleep_for(std::chrono::milliseconds(n));
}

//! Sleep for specified seconds
inline void Sleep(int64_t n) {
    std::this_thread::sleep_for(std::chrono::seconds(n));
}

//! High resolution clock for performance measurement
using SteadyClock = std::chrono::steady_clock;
using SteadyTimePoint = std::chrono::time_point<SteadyClock>;

inline SteadyTimePoint SteadyNow() {
    return SteadyClock::now();
}

//! Convert duration to milliseconds
template<typename Duration>
int64_t ToMillis(Duration d) {
    return std::chrono::duration_cast<std::chrono::milliseconds>(d).count();
}

//! Convert duration to seconds
template<typename Duration>
int64_t ToSeconds(Duration d) {
    return std::chrono::duration_cast<std::chrono::seconds>(d).count();
}

//! Format time as ISO 8601 string
std::string FormatISO8601DateTime(int64_t time);
std::string FormatISO8601Date(int64_t time);

//! Parse ISO 8601 datetime string
int64_t ParseISO8601DateTime(const std::string& str);

//! Mockable time for testing
class MockableClock {
public:
    using time_point = std::chrono::time_point<MockableClock, std::chrono::seconds>;
    using duration = std::chrono::seconds;

    static time_point now() noexcept {
        return time_point{std::chrono::seconds{s_mock_time.load(std::memory_order_relaxed)}};
    }

    static void SetMockTime(int64_t time) {
        s_mock_time.store(time, std::memory_order_relaxed);
    }

    static int64_t GetMockTime() {
        return s_mock_time.load(std::memory_order_relaxed);
    }

private:
    static inline std::atomic<int64_t> s_mock_time{0};
};

//! Set mock time for testing
inline void SetMockTime(int64_t time) {
    MockableClock::SetMockTime(time);
}

//! Get mock time for testing
inline int64_t GetMockTime() {
    return MockableClock::GetMockTime();
}

//! Get adjusted time (accounts for network time offset)
int64_t GetAdjustedTime();

} // namespace util

#endif // ALIAS_UTIL_TIME_H
