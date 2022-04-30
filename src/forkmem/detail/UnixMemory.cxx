
#include <string>
#if defined(__unix__) || defined(__APPLE__)
#include <cerrno>
#include <cstdio>
#include <stdexcept>
#include <system_error>
#include <unistd.h>

#include <algorithm>
#include <random>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "forkmem/detail/UnixMemory.hxx"

namespace forkmem {

template <class T = std::mt19937, std::size_t N = T::state_size * sizeof(typename T::result_type)>
auto ProperlySeededRandomEngine() {
    std::random_device source;
    std::random_device::result_type random_data[(N - 1) / sizeof(source()) + 1];
    std::generate(std::begin(random_data), std::end(random_data), std::ref(source));
    std::seed_seq seeds(std::begin(random_data), std::end(random_data));
    return T(seeds);
}

UnixMemory::UnixMemory(std::size_t size) : handle{.size = size} {
#define __APPLE__
#if defined(__APPLE__)
    // TODO: be smarter about it
    static auto gen = ProperlySeededRandomEngine<>();
    auto shm_name = "/" + std::to_string(gen());
    handle.handle = shm_open(shm_name.data(), O_RDWR | O_CREAT | O_EXCL, 0600);
#else
    handle.handle = ::memfd_create("anon_memory", 0);
#endif
    if (handle.handle < 0)
        throw std::runtime_error{std::system_category().message(errno)};
#if defined(__APPLE__)
    // shm_open defaults to CLOEXEC, undo it.
    auto flags = fcntl(handle.handle, F_GETFD);
    flags &= ~FD_CLOEXEC;
    fcntl(handle.handle, F_SETFD, flags);
    // directly unlink to avoid anyone observing our memory
    shm_unlink(shm_name.data());
#endif

    if (ftruncate(handle.handle, handle.size) != 0)
        throw std::runtime_error{std::system_category().message(errno)};

    handle.memory = ::mmap(nullptr, handle.size, PROT_READ | PROT_WRITE, MAP_SHARED, handle.handle, 0);

    if (handle.memory == MAP_FAILED)
        throw std::runtime_error{std::system_category().message(errno)};
}

#ifndef MAP_FIXED_NOREPLACE
// Apple...
#define MAP_FIXED_NOREPLACE MAP_FIXED
#endif

void UnixMemory::make_reachable(const Handle& handle) {
    const auto res = ::mmap(handle.memory, handle.size, PROT_READ | PROT_WRITE,
                            MAP_SHARED | MAP_FIXED_NOREPLACE, handle.handle, 0);

    if (res == nullptr)
        throw std::runtime_error{std::system_category().message(errno)};
}

unsigned char* UnixMemory::data() { return reinterpret_cast<unsigned char*>(handle.memory); }

UnixMemory::~UnixMemory() { ::munmap(handle.memory, handle.size); }
} // namespace forkmem
#endif