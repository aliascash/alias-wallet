// SPDX-FileCopyrightText: © 2025 ALIAS Developers
// SPDX-FileCopyrightText: © 2020 Alias Developers
// Copyright (c) 2009-2022 The Bitcoin Core developers
// SPDX-License-Identifier: MIT

#include <util/time.h>

#include <ctime>
#include <cstdio>
#include <atomic>
#include <mutex>
#include <set>

namespace util {

static std::atomic<int64_t> nTimeOffset{0};

std::string FormatISO8601DateTime(int64_t time) {
    std::time_t t = static_cast<std::time_t>(time);
    std::tm* tm = std::gmtime(&t);
    if (!tm) return "";
    
    char buf[32];
    std::strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%SZ", tm);
    return buf;
}

std::string FormatISO8601Date(int64_t time) {
    std::time_t t = static_cast<std::time_t>(time);
    std::tm* tm = std::gmtime(&t);
    if (!tm) return "";
    
    char buf[16];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d", tm);
    return buf;
}

int64_t ParseISO8601DateTime(const std::string& str) {
    std::tm tm = {};
    int year, month, day, hour, min, sec;
    
    if (sscanf(str.c_str(), "%d-%d-%dT%d:%d:%dZ", 
               &year, &month, &day, &hour, &min, &sec) != 6) {
        return 0;
    }
    
    tm.tm_year = year - 1900;
    tm.tm_mon = month - 1;
    tm.tm_mday = day;
    tm.tm_hour = hour;
    tm.tm_min = min;
    tm.tm_sec = sec;
    tm.tm_isdst = 0;
    
#ifdef _WIN32
    return _mkgmtime(&tm);
#else
    return timegm(&tm);
#endif
}

static std::mutex cs_nTimeOffset;
static std::set<int64_t> setTimeOffsets;

void AddTimeData(int64_t nOffset) {
    std::lock_guard<std::mutex> lock(cs_nTimeOffset);
    
    setTimeOffsets.insert(nOffset);
    
    if (setTimeOffsets.size() >= 5) {
        auto it = setTimeOffsets.begin();
        std::advance(it, setTimeOffsets.size() / 2);
        nTimeOffset.store(*it, std::memory_order_relaxed);
    }
}

int64_t GetTimeOffset() {
    return nTimeOffset.load(std::memory_order_relaxed);
}

int64_t GetAdjustedTime() {
    int64_t mock = GetMockTime();
    if (mock > 0) return mock;
    return GetTime() + GetTimeOffset();
}

} // namespace util
