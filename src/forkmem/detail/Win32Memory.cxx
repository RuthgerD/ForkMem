
#if defined(_WIN32)
#include <stdexcept>
#include <system_error>

#include "forkmem/detail/Win32Memory.hxx"

#include <windows.h>

#include <memoryapi.h>
#include <winbase.h>

namespace forkmem {

void throw_system_error() { throw std::runtime_error{std::system_category().message(::GetLastError())}; };

Win32Memory::Win32Memory(std::size_t size) : handle{.size = size} {
    SECURITY_ATTRIBUTES sec = {0};
    sec.nLength = sizeof(SECURITY_ATTRIBUTES);
    sec.bInheritHandle = 1;

    handle.handle = ::CreateFileMappingA(NULL, &sec, PAGE_READWRITE, 0, handle.size, NULL);
    if (handle.handle == nullptr) {
        throw_system_error();
    }

    handle.memory = ::MapViewOfFileEx(handle.handle, FILE_MAP_ALL_ACCESS, 0, 0, handle.size, NULL);

    if (handle.memory == nullptr) {
        throw_system_error();
    }
}

void Win32Memory::make_reachable(const Handle& handle) {
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