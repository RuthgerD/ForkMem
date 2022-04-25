
#ifndef FORKMEM_ALLOCATOR_HXX
#define FORKMEM_ALLOCATOR_HXX

#include <utility>
#ifdef __has_include
#if __has_include(<memory_resource>)
#include <memory_resource>
namespace forkmem {
template <class T> using polymorphic_allocator = std::pmr::polymorphic_allocator<T>;
using memory_resource = std::pmr::memory_resource;
} // namespace forkmem
#elif __has_include(<experimental/memory_resource>)
#include <experimental/memory_resource>
namespace forkmem {
template <class T> using polymorphic_allocator = std::experimental::pmr::polymorphic_allocator<T>;
using memory_resource = std::experimental::pmr::memory_resource;
} // namespace forkmem
#else
#error "Missing <memory_resource>"
#endif
#endif

namespace forkmem {

template <class T, class... Args> T* new_object(polymorphic_allocator<T> alloc, Args... args) {
    auto* p = alloc.allocate(1);
    try {
        alloc.construct(p, std::forward<Args>(args)...);
    } catch (...) {
        alloc.deallocate(p, 1);
        throw;
    }
    return p;
}

template <class T> void delete_object(polymorphic_allocator<T> alloc, T* p) {
    p->~T();
    alloc.deallocate(p, 1);
}

} // namespace forkmem

#endif