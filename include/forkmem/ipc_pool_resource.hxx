#pragma once

#include <mutex>
#include "forkmem/Mutex.hxx"
#include "forkmem/allocator.hxx"

namespace frkm {
class ipc_pool_resource : public memory_resource {

    monotonic_buffer_resource monotonic;
    unsynchronized_pool_resource pool_resource;
    mutable IpcMutex mut;

  public:
    explicit ipc_pool_resource(void* buffer, std::size_t size)
        : monotonic{buffer, size}, pool_resource{&monotonic} {}
    explicit ipc_pool_resource(memory_resource* upstream) : monotonic{upstream}, pool_resource{&monotonic} {}
    ipc_pool_resource(const ipc_pool_resource&) = delete;

    void* do_allocate(std::size_t bytes, std::size_t alignment) override {
        std::scoped_lock lk{mut};
        return pool_resource.allocate(bytes, alignment);
    }

    void do_deallocate(void* p, std::size_t bytes, std::size_t alignment) override {
        std::scoped_lock lk{mut};
        pool_resource.deallocate(p, bytes, alignment);
    }

    bool do_is_equal(const memory_resource& other) const noexcept override {
        std::scoped_lock lk{mut};
        return pool_resource.is_equal(other);
    }
};
} // namespace frkm