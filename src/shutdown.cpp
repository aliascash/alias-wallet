// SPDX-FileCopyrightText: © 2025 ALIAS Developers
// SPDX-FileCopyrightText: © 2020 Alias Developers
// SPDX-FileCopyrightText: © 2009 Bitcoin Developers
// SPDX-FileCopyrightText: © 2009 Satoshi Nakamoto
//
// SPDX-License-Identifier: MIT

#include "shutdown.h"

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>

#ifndef WIN32
#include <fcntl.h>
#include <unistd.h>
#endif

namespace util {

SignalInterrupt::SignalInterrupt() {
#ifndef WIN32
    int fds[2];
    if (pipe(fds) == 0) {
        m_pipe_r = fds[0];
        m_pipe_w = fds[1];
        fcntl(m_pipe_r, F_SETFL, fcntl(m_pipe_r, F_GETFL) | O_NONBLOCK);
        fcntl(m_pipe_w, F_SETFL, fcntl(m_pipe_w, F_GETFL) | O_NONBLOCK);
    }
#endif
}

SignalInterrupt::operator bool() const {
    return m_flag.load(std::memory_order_acquire);
}

bool SignalInterrupt::operator()() {
    bool expected = false;
    if (m_flag.compare_exchange_strong(expected, true, std::memory_order_acq_rel)) {
#ifndef WIN32
        if (m_pipe_w != -1) {
            char dummy = 0;
            [[maybe_unused]] auto result = write(m_pipe_w, &dummy, 1);
        }
#endif
        return true;
    }
    return false;
}

bool SignalInterrupt::wait() {
    while (!m_flag.load(std::memory_order_acquire)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    return true;
}

bool SignalInterrupt::reset() {
    m_flag.store(false, std::memory_order_release);
#ifndef WIN32
    if (m_pipe_r != -1) {
        char buf[256];
        while (read(m_pipe_r, buf, sizeof(buf)) > 0) {}
    }
#endif
    return true;
}

bool SignalInterrupt::sleep_for(std::chrono::milliseconds duration) {
    auto start = std::chrono::steady_clock::now();
    while (!m_flag.load(std::memory_order_acquire)) {
        auto elapsed = std::chrono::steady_clock::now() - start;
        if (elapsed >= duration) return true;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    return false;
}

} // namespace util

static std::atomic<bool> fRequestShutdown{false};
static std::mutex cs_shutdown;
static std::condition_variable cv_shutdown;
static std::function<void()> g_shutdown_callback;

void StartShutdown() {
    fRequestShutdown.store(true, std::memory_order_release);
    cv_shutdown.notify_all();
    if (g_shutdown_callback) {
        g_shutdown_callback();
    }
}

void AbortShutdown() {
    fRequestShutdown.store(false, std::memory_order_release);
}

bool ShutdownRequested() {
    return fRequestShutdown.load(std::memory_order_acquire);
}

void WaitForShutdown() {
    std::unique_lock<std::mutex> lock(cs_shutdown);
    cv_shutdown.wait(lock, [] { return fRequestShutdown.load(std::memory_order_acquire); });
}

void SetShutdownCallback(std::function<void()> callback) {
    g_shutdown_callback = std::move(callback);
}
