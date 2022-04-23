#if defined(__unix__) || defined(__APPLE__)
#include <dlfcn.h>
#include <stdlib.h>
#include <sys/mman.h>
#include "forkmem/unix/IpcMemory.hxx"

namespace frkm {
IpcMemory::IpcMemory(std::size_t size) {
    mmap_buffer = mmap(0, size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    mmap_size = size;

    auto usable_buf = reinterpret_cast<char*>(mmap_buffer);
    resource = new (mmap_buffer)
        ipc_pool_resource{usable_buf + sizeof(ipc_pool_resource), mmap_size - sizeof(ipc_pool_resource)};
}

polymorphic_allocator<std::byte> IpcMemory::get_allocator() { return resource; }

IpcMemory::~IpcMemory() {
    resource->~ipc_pool_resource();
    munmap(mmap_buffer, mmap_size);
}

} // namespace frkm
#endif