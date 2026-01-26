// SPDX-FileCopyrightText: © 2025 ALIAS Developers
// SPDX-FileCopyrightText: © 2020 Alias Developers
// Copyright (c) 2018-2022 The Bitcoin Core developers
// SPDX-License-Identifier: MIT

#ifndef ALIAS_INTERFACES_HANDLER_H
#define ALIAS_INTERFACES_HANDLER_H

#include <memory>
#include <functional>

namespace interfaces {

//! Generic interface for managing an event handler or callback function
//! registered with another interface. Has a single disconnect method to cancel
//! the registration and prevent any future notifications.
class Handler {
public:
    virtual ~Handler() = default;

    //! Disconnect the handler
    virtual void disconnect() = 0;
};

//! Return handler wrapping a cleanup function
std::unique_ptr<Handler> MakeCleanupHandler(std::function<void()> cleanup);

//! Simple handler implementation that calls cleanup function on disconnect
class CleanupHandler : public Handler {
public:
    explicit CleanupHandler(std::function<void()> cleanup) : m_cleanup(std::move(cleanup)) {}

    ~CleanupHandler() override { 
        if (m_cleanup) {
            m_cleanup(); 
        }
    }

    void disconnect() override {
        if (m_cleanup) {
            m_cleanup();
            m_cleanup = nullptr;
        }
    }

private:
    std::function<void()> m_cleanup;
};

inline std::unique_ptr<Handler> MakeCleanupHandler(std::function<void()> cleanup) {
    return std::make_unique<CleanupHandler>(std::move(cleanup));
}

} // namespace interfaces

#endif // ALIAS_INTERFACES_HANDLER_H
