#pragma once

#include <atomic>
#include <iostream>
#include <thread>
#include "forkmem/Atomic.hxx"
namespace frkm {

class IpcMutex {
    IpcAtomic<bool> flag;

  public:
    bool try_lock() noexcept { return !flag.exchange(true, std::memory_order_acquire); }

    void lock() noexcept {
        while (!try_lock()) {
            std::cout << "actually waiting on atomic \n";
            flag.wait(true, std::memory_order_acquire);
            std::cout << "wait done\n";
        }
    }

    void unlock() noexcept {
        flag.store(false, std::memory_order_release);
        flag.notify_one();
        std::cout << "notified one\n";
    }
};
} // namespace frkm