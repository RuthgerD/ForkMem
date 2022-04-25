
#ifndef FORKMEM_MEMORY_HXX
#define FORKMEM_MEMORY_HXX

#include "forkmem/Allocator.hxx"

#if defined(__unix__) || defined(__APPLE__)
#include "forkmem/detail/UnixMemory.hxx"
namespace forkmem {
using NativeMemory = forkmem::UnixMemory;
}
#elif defined(_WIN32)
#include "forkmem/detail/Win32Memory.hxx"
namespace forkmem {
using NativeMemory = forkmem::Win32Memory;
}
#else
#error "No shareable memory implementation available"
#endif

namespace forkmem {

class Memory {
    NativeMemory memory;
    memory_resource* resource;

  public:
    Memory(std::size_t);

    memory_resource* get_resource();

    const NativeMemory& get_native() const { return memory; }

    ~Memory();
};

} // namespace forkmem

#endif