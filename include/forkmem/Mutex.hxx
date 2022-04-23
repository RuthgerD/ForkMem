#pragma once

#include <atomic>
#include <thread>
#include "forkmem/Atomic.hxx"
namespace frkm {

class IpcMutex {
    IpcAtomic<bool> flag;

  public:
    bool try_lock() noexcept { return !flag.exchange(true, std::memory_order_acquire); }

    void lock() noexcept {

        while (!try_lock())
            flag.wait(true, std::memory_order_acquire);
    }

    void unlock() noexcept {
        flag.store(false, std::memory_order_release);
        flag.notify_one();
    }
};
} // namespace frkm