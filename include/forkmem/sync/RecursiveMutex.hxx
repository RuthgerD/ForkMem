

#ifndef FORKMEM_RECURSIVEMUTEX_HXX
#define FORKMEM_RECURSIVEMUTEX_HXX
#include <cstddef>
#include <cstdint>
#include "forkmem/sync/Atomic.hxx"
#include "forkmem/sync/Mutex.hxx"
namespace forkmem {

class RecursiveMutex {
    Atomic<std::uint64_t> pid;
    Atomic<std::uint64_t> tid;

    std::size_t recursion;

  public:
    bool try_lock();

    void lock();

    void unlock();
};

} // namespace forkmem
#endif