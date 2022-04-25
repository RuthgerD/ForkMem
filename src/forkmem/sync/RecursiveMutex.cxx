#include <atomic>
#include <cstdint>
#include <thread>
#include "forkmem/sync/Atomic.hxx"
#include "forkmem/sync/RecursiveMutex.hxx"

#if defined(__unix__) || defined(__APPLE__)
#include <pthread.h>
#include <unistd.h>

namespace forkmem {

std::uint64_t process_id() { return ::getpid(); }

std::uint64_t thread_id() { return ::pthread_self(); }

} // namespace forkmem

#elif defined(_WIN32)

#include <processthreadsapi.h>
#include <windows.h>
namespace forkmem {
std::uint64_t process_id() { return 0; }

std::uint64_t thread_id() { return ::GetCurrentThreadId(); }
} // namespace forkmem

#endif

namespace forkmem {

bool RecursiveMutex::try_lock() {

    auto self_pid = forkmem::process_id();
    auto self_tid = forkmem::thread_id();

    std::uint64_t expected = 0;
    if (pid.compare_exchange_weak(expected, self_pid, std::memory_order_seq_cst, std::memory_order_seq_cst)) {
        tid.store(self_tid);
        recursion += 1;
        return true;
    } else if (pid.load() == self_pid && tid.load() == self_tid) {
        recursion += 1;
        return true;
    }

    return false;
}

void RecursiveMutex::lock() {
    while (!try_lock())
        std::this_thread::yield();
}

void RecursiveMutex::unlock() {
    if (--recursion != 0)
        return;

    pid.store(0);
    tid.store(0);
}

} // namespace forkmem