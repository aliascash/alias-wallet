// SPDX-FileCopyrightText: © 2025 ALIAS Developers
// SPDX-FileCopyrightText: © 2020 Alias Developers
// Copyright (c) 2009-2022 The Bitcoin Core developers
// SPDX-License-Identifier: MIT

#ifndef ALIAS_LOGGING_H
#define ALIAS_LOGGING_H

#include <atomic>
#include <cstdint>
#include <functional>
#include <list>
#include <mutex>
#include <string>
#include <vector>

namespace BCLog {

enum LogFlags : uint32_t {
    NONE        = 0,
    NET         = (1 << 0),
    TOR         = (1 << 1),
    MEMPOOL     = (1 << 2),
    HTTP        = (1 << 3),
    BENCH       = (1 << 4),
    ZMQ         = (1 << 5),
    WALLETDB    = (1 << 6),
    RPC         = (1 << 7),
    ESTIMATEFEE = (1 << 8),
    ADDRMAN     = (1 << 9),
    SELECTCOINS = (1 << 10),
    REINDEX     = (1 << 11),
    CMPCTBLOCK  = (1 << 12),
    RAND        = (1 << 13),
    PRUNE       = (1 << 14),
    PROXY       = (1 << 15),
    MEMPOOLREJ  = (1 << 16),
    LIBEVENT    = (1 << 17),
    COINDB      = (1 << 18),
    QT          = (1 << 19),
    LEVELDB     = (1 << 20),
    STAKING     = (1 << 21),
    SMSG        = (1 << 22),
    ANON        = (1 << 23),
    HDWALLET    = (1 << 24),
    ALL         = ~(uint32_t)0,
};

enum class Level {
    Trace,
    Debug,
    Info,
    Warning,
    Error,
};

class Logger {
public:
    bool m_print_to_console{false};
    bool m_print_to_file{true};
    bool m_log_timestamps{true};
    bool m_log_time_micros{false};
    bool m_log_threadnames{false};
    bool m_log_sourcelocations{false};
    std::atomic<bool> m_reopen_file{false};
    std::atomic<uint32_t> m_categories{0};
    Level m_log_level{Level::Debug};

    void EnableCategory(LogFlags category);
    void DisableCategory(LogFlags category);
    bool WillLogCategory(LogFlags category) const;
    bool WillLogCategoryLevel(LogFlags category, Level level) const;
    
    std::string LogCategoriesString() const;
    bool SetLogLevel(const std::string& level);
    
    bool StartLogging();
    void DisconnectTestLogger();
    
    void LogPrintStr(const std::string& str, const std::string& log_prefix = "", const std::string& file = "", int line = 0);
    
    bool Enabled() const {
        return m_print_to_console || m_print_to_file;
    }

    std::list<std::function<void(const std::string&)>> PushBackCallback(std::function<void(const std::string&)> callback);
    void DeleteCallback(std::list<std::function<void(const std::string&)>>::iterator it);

private:
    mutable std::mutex m_cs;
    FILE* m_file{nullptr};
    std::list<std::function<void(const std::string&)>> m_print_callbacks;
    std::string m_buffer;
    bool m_buffering{true};
    
    std::string GetTimestamp() const;
    void Flush();
};

} // namespace BCLog

BCLog::Logger& LogInstance();

static inline bool LogAcceptCategory(BCLog::LogFlags flag, BCLog::Level level) {
    return LogInstance().WillLogCategoryLevel(flag, level);
}

static inline bool LogAcceptCategory(BCLog::LogFlags flag) {
    return LogInstance().WillLogCategory(flag);
}

bool GetLogCategory(BCLog::LogFlags& flag, const std::string& category);
std::string GetLogCategoryString(BCLog::LogFlags flag);

template<typename... Args>
static inline void LogPrint(BCLog::LogFlags category, const char* fmt, const Args&... args);

template<typename... Args>
static inline void LogPrintLevel(BCLog::LogFlags category, BCLog::Level level, const char* fmt, const Args&... args);

#define LogPrintf(...) LogPrint(BCLog::NONE, __VA_ARGS__)
#define LogPrintfLevel(level, ...) LogPrintLevel(BCLog::NONE, level, __VA_ARGS__)

#endif // ALIAS_LOGGING_H
