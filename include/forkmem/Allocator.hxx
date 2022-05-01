
#ifndef FORKMEM_ALLOCATOR_HXX
#define FORKMEM_ALLOCATOR_HXX

#include <cstddef>
#include <memory>
#include <utility>

namespace forkmem {
class MemoryPool {
    void* impl;

  public:
    MemoryPool(std::size_t size);
    [[nodiscard]] void* allocate(std::size_t bytes, std::size_t alignment = alignof(std::max_align_t));
    void deallocate(void* p, std::size_t bytes, std::size_t alignment = alignof(std::max_align_t));
};

template <class T, class Alloc> class StaticAllocator {
    Alloc* alloc;

  public:
    StaticAllocator(Alloc* alloc) : alloc{alloc} {}
    StaticAllocator(const StaticAllocator& rhs) = default;

    template <class U> StaticAllocator(const StaticAllocator<U, Alloc>& rhs) : alloc{rhs.get_allocator()} {};

    using value_type = T;

    [[nodiscard]] Alloc* get_allocator() const { return alloc; }

    [[nodiscard]] T* allocate(std::size_t n) {
        return static_cast<T*>(alloc->allocate(sizeof(T) * n, alignof(T)));
    }
    void deallocate(T* p, std::size_t n) { alloc->deallocate(p, sizeof(T) * n, alignof(T)); }

    template <class U, class... Args> void construct(U* p, Args&&... args) {
        new (p) U{std::forward<Args>(args)...};
    }

    template <class... Args> T* new_object(Args... args) {
        auto* p = allocate(1);
        try {
            construct(p, std::forward<Args>(args)...);
        } catch (...) {
            deallocate(p, 1);
            throw;
        }
        return p;
    }

    void delete_object(T* p) {
        p->~T();
        deallocate(p, 1);
    }
};

template <class T> using PoolAllocator = StaticAllocator<T, MemoryPool>;

} // namespace forkmem

#endif