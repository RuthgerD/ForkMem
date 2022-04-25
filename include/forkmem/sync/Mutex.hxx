
#ifndef FORKMEM_MUTEX_HXX
#define FORKMEM_MUTEX_HXX

#include "forkmem/sync/Atomic.hxx"

namespace forkmem {

class Mutex {
    Atomic<bool> flag;

  public:
    bool try_lock();

    void lock();

    void unlock();
};
} // namespace forkmem
#endif