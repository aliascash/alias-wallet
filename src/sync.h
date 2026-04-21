// SPDX-FileCopyrightText: © 2025 ALIAS Developers
// SPDX-FileCopyrightText: © 2020 Alias Developers
// SPDX-FileCopyrightText: © 2016 SpectreCoin Developers
// SPDX-FileCopyrightText: © 2009 Bitcoin Developers
// SPDX-FileCopyrightText: © 2009 Satoshi Nakamoto
//
// SPDX-License-Identifier: MIT

#ifndef ALIAS_SYNC_H
#define ALIAS_SYNC_H

#include "threadsafety.h"

#include <condition_variable>
#include <mutex>
#include <shared_mutex>
#include <thread>

template <typename PARENT>
class LOCKABLE AnnotatedMixin : public PARENT {
public:
    void lock() EXCLUSIVE_LOCK_FUNCTION() {
        PARENT::lock();
    }

    void unlock() UNLOCK_FUNCTION() {
        PARENT::unlock();
    }

    bool try_lock() EXCLUSIVE_TRYLOCK_FUNCTION(true) {
        return PARENT::try_lock();
    }
};

using RecursiveMutex = AnnotatedMixin<std::recursive_mutex>;
using Mutex = AnnotatedMixin<std::mutex>;

using CCriticalSection = RecursiveMutex;
using CWaitableCriticalSection = Mutex;
using CConditionVariable = std::condition_variable;
using GlobalMutex = Mutex;

#ifdef DEBUG_LOCKORDER
void EnterCritical(const char* pszName, const char* pszFile, int nLine, void* cs, bool fTry = false);
void LeaveCritical();
std::string LocksHeld();
void AssertLockHeldInternal(const char* pszName, const char* pszFile, int nLine, void* cs);
void AssertLockNotHeldInternal(const char* pszName, const char* pszFile, int nLine, void* cs);
void DeleteLock(void* cs);
bool LockStackEmpty();
#else
inline void EnterCritical(const char* pszName, const char* pszFile, int nLine, void* cs, bool fTry = false) {}
inline void LeaveCritical() {}
inline void AssertLockHeldInternal(const char* pszName, const char* pszFile, int nLine, void* cs) {}
inline void AssertLockNotHeldInternal(const char* pszName, const char* pszFile, int nLine, void* cs) {}
inline void DeleteLock(void* cs) {}
inline bool LockStackEmpty() { return true; }
#endif

#define AssertLockHeld(cs) AssertLockHeldInternal(#cs, __FILE__, __LINE__, &cs)
#define AssertLockNotHeld(cs) AssertLockNotHeldInternal(#cs, __FILE__, __LINE__, &cs)

#ifdef DEBUG_LOCKCONTENTION
void PrintLockContention(const char* pszName, const char* pszFile, int nLine);
#endif

template <typename MutexType>
class SCOPED_LOCKABLE UniqueLock {
private:
    std::unique_lock<MutexType> lock;

    void Enter(const char* pszName, const char* pszFile, int nLine) {
        EnterCritical(pszName, pszFile, nLine, static_cast<void*>(lock.mutex()));
#ifdef DEBUG_LOCKCONTENTION
        if (!lock.try_lock()) {
            PrintLockContention(pszName, pszFile, nLine);
#endif
            lock.lock();
#ifdef DEBUG_LOCKCONTENTION
        }
#endif
    }

    bool TryEnter(const char* pszName, const char* pszFile, int nLine) {
        EnterCritical(pszName, pszFile, nLine, static_cast<void*>(lock.mutex()), true);
        lock.try_lock();
        if (!lock.owns_lock()) {
            LeaveCritical();
        }
        return lock.owns_lock();
    }

public:
    UniqueLock(MutexType& mutexIn, const char* pszName, const char* pszFile, int nLine, bool fTry = false) EXCLUSIVE_LOCK_FUNCTION(mutexIn)
        : lock(mutexIn, std::defer_lock) {
        if (fTry) {
            TryEnter(pszName, pszFile, nLine);
        } else {
            Enter(pszName, pszFile, nLine);
        }
    }

    UniqueLock(MutexType* pmutexIn, const char* pszName, const char* pszFile, int nLine, bool fTry = false) EXCLUSIVE_LOCK_FUNCTION(pmutexIn) {
        if (!pmutexIn) return;

        lock = std::unique_lock<MutexType>(*pmutexIn, std::defer_lock);
        if (fTry) {
            TryEnter(pszName, pszFile, nLine);
        } else {
            Enter(pszName, pszFile, nLine);
        }
    }

    ~UniqueLock() UNLOCK_FUNCTION() {
        if (lock.owns_lock()) {
            LeaveCritical();
        }
    }

    operator bool() const {
        return lock.owns_lock();
    }
};

template <typename MutexType>
using DebugLock = UniqueLock<MutexType>;

using CMutexLock = UniqueLock<CCriticalSection>;
using CCriticalBlock = CMutexLock;

#define LOCK(cs) UniqueLock<decltype(cs)> UNIQUE_NAME(criticalblock)(cs, #cs, __FILE__, __LINE__)
#define LOCK2(cs1, cs2)                                               \
    UniqueLock<decltype(cs1)> UNIQUE_NAME(criticalblock1)(cs1, #cs1, __FILE__, __LINE__); \
    UniqueLock<decltype(cs2)> UNIQUE_NAME(criticalblock2)(cs2, #cs2, __FILE__, __LINE__)
#define TRY_LOCK(cs, name) UniqueLock<decltype(cs)> name(cs, #cs, __FILE__, __LINE__, true)
#define WAIT_LOCK(cs, name) UniqueLock<decltype(cs)> name(cs, #cs, __FILE__, __LINE__)

#define UNIQUE_NAME_HELPER(name, line) name##line
#define UNIQUE_NAME(name) UNIQUE_NAME_HELPER(name, __LINE__)

#define ENTER_CRITICAL_SECTION(cs)                            \
    {                                                         \
        EnterCritical(#cs, __FILE__, __LINE__, (void*)(&cs)); \
        (cs).lock();                                          \
    }

#define LEAVE_CRITICAL_SECTION(cs) \
    {                              \
        (cs).unlock();             \
        LeaveCritical();           \
    }

class CSemaphore {
private:
    std::condition_variable condition;
    std::mutex mutex;
    int value;

public:
    explicit CSemaphore(int init) : value(init) {}

    void wait() {
        std::unique_lock<std::mutex> lock(mutex);
        condition.wait(lock, [this] { return value >= 1; });
        value--;
    }

    bool try_wait() {
        std::unique_lock<std::mutex> lock(mutex);
        if (value < 1)
            return false;
        value--;
        return true;
    }

    void post() {
        {
            std::unique_lock<std::mutex> lock(mutex);
            value++;
        }
        condition.notify_one();
    }
};

class CSemaphoreGrant {
private:
    CSemaphore* sem{nullptr};
    bool fHaveGrant{false};

public:
    void Acquire() {
        if (fHaveGrant)
            return;
        sem->wait();
        fHaveGrant = true;
    }

    void Release() {
        if (!fHaveGrant)
            return;
        sem->post();
        fHaveGrant = false;
    }

    bool TryAcquire() {
        if (!fHaveGrant && sem->try_wait())
            fHaveGrant = true;
        return fHaveGrant;
    }

    void MoveTo(CSemaphoreGrant& grant) {
        grant.Release();
        grant.sem = sem;
        grant.fHaveGrant = fHaveGrant;
        sem = nullptr;
        fHaveGrant = false;
    }

    CSemaphoreGrant() = default;

    explicit CSemaphoreGrant(CSemaphore& sema, bool fTry = false) : sem(&sema), fHaveGrant(false) {
        if (fTry) {
            TryAcquire();
        } else {
            Acquire();
        }
    }

    ~CSemaphoreGrant() {
        Release();
    }

    operator bool() const {
        return fHaveGrant;
    }
};

#endif // ALIAS_SYNC_H
