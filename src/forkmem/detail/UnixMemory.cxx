
#if defined(__unix__) || defined(__APPLE__)
#include <cerrno>
#include <cstdio>
#include <stdexcept>
#include <system_error>
#include <sys/mman.h>
#include "forkmem/detail/UnixMemory.hxx"

namespace forkmem {

UnixMemory::UnixMemory(std::size_t size) : size{size} {
    memory = ::mmap(0, size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    if (memory == MAP_FAILED) {
        throw std::runtime_error{std::system_category().message(errno)};
    }
}

unsigned char* UnixMemory::data() { return reinterpret_cast<unsigned char*>(memory); }

UnixMemory::~UnixMemory() { ::munmap(memory, size); }
} // namespace forkmem
#endif