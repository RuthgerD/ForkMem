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
            std::this_thread::yield();
    }

    void unlock() noexcept { flag.store(false, std::memory_order_release); }
};
} // namespace frkm