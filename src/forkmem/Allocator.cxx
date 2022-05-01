#include <cstddef>
#include <iostream>
#include <new>
#include <boost/interprocess/mem_algo/rbtree_best_fit.hpp>
#include <boost/interprocess/mem_algo/simple_seq_fit.hpp>

#include "forkmem/Allocator.hxx"
#include "forkmem/sync/Mutex.hxx"
#include "forkmem/sync/RecursiveMutex.hxx"

namespace forkmem {

struct mutex_family {
    using mutex_type = forkmem::Mutex;
    using recursive_mutex_type = forkmem::RecursiveMutex;
};

using underlying = boost::interprocess::rbtree_best_fit<mutex_family, void*>;

MemoryPool::MemoryPool(std::size_t size) {
    std::uintptr_t start_addr = reinterpret_cast<std::uintptr_t>(this) + sizeof(MemoryPool);
    constexpr std::uintptr_t mask = alignof(underlying) - 1;
    auto aligned_addr = ((start_addr & mask) == 0) ? start_addr : (start_addr & ~mask) + alignof(underlying);
    impl = new (reinterpret_cast<unsigned char*>(aligned_addr)) underlying{size - sizeof(underlying), 0};
}

void* MemoryPool::allocate(std::size_t bytes, std::size_t alignment) {
    return reinterpret_cast<underlying*>(impl)->allocate_aligned(bytes, alignment);
}

void MemoryPool::deallocate(void* p, std::size_t, std::size_t) {
    reinterpret_cast<underlying*>(impl)->deallocate(p);
}

} // namespace forkmem