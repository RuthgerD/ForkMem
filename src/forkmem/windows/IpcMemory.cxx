#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#include "forkmem/windows/IpcMemory.hxx"

#include <windows.h>

#include <memoryapi.h>
#include <winbase.h>

#include <iostream>
#include <stdexcept>

namespace frkm {
IpcMemory::IpcMemory(std::size_t size) {
    std::cout << "IpcMemory windows CTOR" << std::endl;
    mapping_size = size;
    void* ptr_base = VirtualAlloc(NULL, mapping_size, MEM_RESERVE, PAGE_READWRITE);
    std::cout << "IpcMemory windows reserved alloc section" << std::endl;

    SECURITY_ATTRIBUTES sec = {0};
    sec.nLength = sizeof(SECURITY_ATTRIBUTES);
    sec.bInheritHandle = 1;

    map_handle = CreateFileMappingA(NULL, &sec, PAGE_READWRITE, mapping_size, mapping_size, NULL);
    if (map_handle == nullptr) {
        std::cout << "failed create file mapping: " << GetLastError() << std::endl;
        throw std::runtime_error{"Failed to create file mapping"};
    }
    std::cout << "IpcMemory windows created file mapping a" << map_handle << std::endl;

    VirtualFree(ptr_base, 0, MEM_RELEASE);

    std::cout << "IpcMemory windows freed memory allocation" << std::endl;

    mmap_buffer = MapViewOfFileEx(map_handle, FILE_MAP_ALL_ACCESS, 0, 0, mapping_size, ptr_base);

    if (mmap_buffer == nullptr) {
        std::cout << "failed to map view: " << GetLastError() << std::endl;
        throw std::runtime_error{"Failed to map view"};
    }

    std::cout << "IpcMemory windows file mapping got mapped" << std::endl;

    auto* usable_buf = reinterpret_cast<unsigned char*>(mmap_buffer);
    std::cout << "usable buf: " << mmap_buffer << std::endl;
    resource = new (mmap_buffer)
        ipc_pool_resource{usable_buf + sizeof(ipc_pool_resource), mapping_size - sizeof(ipc_pool_resource)};
}

IpcMemory::~IpcMemory() { resource->~ipc_pool_resource(); }
} // namespace frkm
#endif