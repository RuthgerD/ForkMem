#include <atomic>
#include <thread>
#include "forkmem/sync/Mutex.hxx"

namespace forkmem {

bool Mutex::try_lock() { return !flag.exchange(true, std::memory_order_acquire); }

void Mutex::lock() {
    while (!try_lock())
        std::this_thread::yield();
}

void Mutex::unlock() { flag.store(false, std::memory_order_release); }
} // namespace forkmem