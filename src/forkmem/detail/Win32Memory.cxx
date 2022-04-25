
#if defined(_WIN32)
#include <stdexcept>
#include "forkmem/detail/Win32Memory.hxx"

#include <windows.h>

#include <memoryapi.h>
#include <winbase.h>

namespace forkmem {

void throw_system_error() { throw std::runtime_error{std::system_category().message(::GetLastError())}; };

Win32Memory::Win32Memory(std::size_t size) : handle{.size = size} {
    void* ptr_base = ::VirtualAlloc(NULL, handle.size, MEM_RESERVE, PAGE_READWRITE);

    if (ptr_base == nullptr) {
        throw_system_error();
    }

    SECURITY_ATTRIBUTES sec = {0};
    sec.nLength = sizeof(SECURITY_ATTRIBUTES);
    sec.bInheritHandle = 1;

    handle.handle = ::CreateFileMappingA(NULL, &sec, PAGE_READWRITE, 0, handle.size, NULL);
    if (handle == nullptr) {
        throw_system_error();
    }

    // This would be quite problematic..
    if (::VirtualFree(ptr_base, 0, MEM_RELEASE) == 0) {
        throw_system_error();
    }

    handle.memory = ::MapViewOfFileEx(handle.handle, FILE_MAP_ALL_ACCESS, 0, 0, handle.size, ptr_base);

    if (handle.memory == nullptr) {
        throw_system_error();
    }
}

Win32Memory::Win32Memory(void* handle, void* memory, std::size_t size) : handle{handle, memory, size} {
    const auto res = MapViewOfFileEx(handle.handle, FILE_MAP_ALL_ACCESS, 0, 0, handle.size, handle.memory);
    if (res == nullptr) {
        throw_system_error();
    }
}

unsigned char* Win32Memory::data() { return reinterpret_cast<unsigned char*>(handle.memory); }

Win32Memory::~Win32Memory() {
    if (::UnmapViewOfFileEx(handle.memory, MEM_UNMAP_WITH_TRANSIENT_BOOST) == 0) {
        throw_system_error();
    }
}

} // namespace forkmem
#endif