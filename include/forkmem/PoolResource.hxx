
#pragma once

#include <mutex>
#include <boost/container/pmr/global_resource.hpp>
#include <boost/container/pmr/memory_resource.hpp>
#include <boost/container/pmr/monotonic_buffer_resource.hpp>
#include <boost/container/pmr/unsynchronized_pool_resource.hpp>
#include <boost/container/throw_exception.hpp>
#include "forkmem/Allocator.hxx"
#include "forkmem/sync/Mutex.hxx"

namespace forkmem {

class SharedNullResource : public boost::container::pmr::memory_resource {
  protected:
    void* do_allocate(std::size_t, std::size_t) override { boost::container::throw_bad_alloc(); }

    void do_deallocate(void*, std::size_t, std::size_t) override { boost::container::throw_bad_alloc(); }

    bool do_is_equal(const memory_resource& other) const noexcept override { return &other == this; };
};

class PoolResource : public memory_resource {

    boost::container::pmr::monotonic_buffer_resource monotonic;
    boost::container::pmr::unsynchronized_pool_resource pool_resource;
    mutable Mutex mut;

    [[no_unique_address]] SharedNullResource null_resource;

  public:
    explicit PoolResource(void* buffer, std::size_t size)
        : monotonic{buffer, size, &null_resource}, pool_resource{&monotonic} {}

    PoolResource(const PoolResource&) = delete;

    void* do_allocate(std::size_t bytes, std::size_t alignment) override {
        std::scoped_lock lk{mut};
        return pool_resource.allocate(bytes, alignment);
    }

    void do_deallocate(void* p, std::size_t bytes, std::size_t alignment) override {
        std::scoped_lock lk{mut};
        pool_resource.deallocate(p, bytes, alignment);
    }

    bool do_is_equal(const memory_resource& other) const noexcept override { return &other == this; }
};
} // namespace forkmem