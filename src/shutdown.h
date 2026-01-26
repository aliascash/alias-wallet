// SPDX-FileCopyrightText: © 2025 ALIAS Developers
// SPDX-FileCopyrightText: © 2020 Alias Developers
// SPDX-FileCopyrightText: © 2009 Bitcoin Developers
// SPDX-FileCopyrightText: © 2009 Satoshi Nakamoto
//
// SPDX-License-Identifier: MIT

#ifndef ALIAS_SHUTDOWN_H
#define ALIAS_SHUTDOWN_H

#include <atomic>
#include <functional>
#include <optional>

namespace util {

class SignalInterrupt {
public:
    SignalInterrupt();
    
    explicit operator bool() const;
    [[nodiscard]] bool operator()();
    
    bool wait();
    bool reset();
    bool sleep_for(std::chrono::milliseconds duration);

private:
    std::atomic<bool> m_flag{false};
#ifndef WIN32
    int m_pipe_r{-1};
    int m_pipe_w{-1};
#endif
};

} // namespace util

void StartShutdown();
void AbortShutdown();
bool ShutdownRequested();

void WaitForShutdown();

void SetShutdownCallback(std::function<void()> callback);

#endif // ALIAS_SHUTDOWN_H
