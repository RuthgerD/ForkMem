
#ifndef FORKMEM_MEMORY_HXX
#define FORKMEM_MEMORY_HXX

#include "forkmem/Allocator.hxx"

#if defined(__unix__) || defined(__APPLE__)
#include "forkmem/detail/UnixMemory.hxx"
namespace forkmem {
using Memory = forkmem::UnixMemory;
}
#elif defined(_WIN32)
#include "forkmem/detail/Win32Memory.hxx"
namespace forkmem {
using Memory = forkmem::Win32Memory;
}
#else
#error "No shareable memory implementation available"
#endif

#endif