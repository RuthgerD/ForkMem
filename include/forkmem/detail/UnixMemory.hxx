

#ifndef FORKMEM_UNIXMEMORY_HXX
#define FORKMEM_UNIXMEMORY_HXX

#include <cstddef>
namespace forkmem {

class UnixMemory {
    void* memory;
    std::size_t size;

  public:
    UnixMemory(std::size_t size);

    unsigned char* data();

    ~UnixMemory();
};
} // namespace forkmem

#endif