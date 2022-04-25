

#ifndef FORKMEM_UNIXMEMORY_HXX
#define FORKMEM_UNIXMEMORY_HXX

#include <cstddef>
namespace forkmem {

class UnixMemory {
    void* memory;
    std::size_t size;

  public:
    using Handle = void*;

    UnixMemory(std::size_t size);

    unsigned char* data();

    const Handle& native_handle() const { return memory; }

    ~UnixMemory();
};
} // namespace forkmem

#endif