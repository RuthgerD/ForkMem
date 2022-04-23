#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#include "forkmem/windows/IpcMemory.hxx"

#include <windows.h>

#include <memoryapi.h>
#include <winbase.h>

namespace frkm {
IpcMemory::IpcMemory(std::size_t size) {
    mapping_size = size;
    void* ptr_base = VirtualAlloc(NULL, mapping_size, MEM_RESERVE, PAGE_READWRITE);

    SECURITY_ATTRIBUTES sec = {0};
    sec.nLength = sizeof(SECURITY_ATTRIBUTES);
    sec.bInheritHandle = 1;

    map_handle = CreateFileMappingA(NULL, &sec, PAGE_READWRITE, 1024, 1024, NULL);

    VirtualFree(ptr_base, 0, MEM_RELEASE);

    mmap_buffer = MapViewOfFileEx(map_handle, FILE_MAP_ALL_ACCESS, 0, 0, 1024, ptr_base);

    auto usable_buf = reinterpret_cast<char*>(mmap_buffer);
    resource = new (mmap_buffer)
        ipc_pool_resource{usable_buf + sizeof(ipc_pool_resource), mapping_size - sizeof(ipc_pool_resource)};
}

polymorphic_allocator<std::byte> IpcMemory::get_allocator() { return resource; }

IpcMemory::~IpcMemory() { resource->~ipc_pool_resource(); }

#endif