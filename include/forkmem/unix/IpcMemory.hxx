#pragma once

#include "forkmem/allocator.hxx"
#include "forkmem/ipc_pool_resource.hxx"
namespace frkm {
struct IpcMemory {

    void* mmap_buffer;
    std::size_t mmap_size;

    ipc_pool_resource* resource;

  public:
    IpcMemory(std::size_t size);

    memory_resource* get_resource();

    ~IpcMemory();
};
} // namespace frkm