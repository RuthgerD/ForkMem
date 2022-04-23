#pragma once

#include "forkmem/allocator.hxx"
#include "forkmem/ipc_pool_resource.hxx"
namespace frkm {
struct IpcMemory {
    void* map_handle;
    void* mmap_buffer;
    std::size_t mapping_size;

    ipc_pool_resource* resource;

  public:
    IpcMemory(std::size_t size);

    polymorphic_allocator<std::byte> get_allocator();

    ~IpcMemory();
};
} // namespace frkm