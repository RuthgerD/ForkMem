

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <string>
#include <thread>
#include "Type.hxx"

void program() {
    std::cout << "program entered" << std::endl;
    auto* userdata_ptr = reinterpret_cast<AllocatingType*>(std::stoll(std::getenv("USERDATA_PTR")));

    printf("user data ptr: %p\n", userdata_ptr);
    std::fflush(stdout);

    auto& to_print = userdata_ptr->str;
    std::cout << to_print << std::endl;

    for (auto c : to_print) {
        userdata_ptr->channel.push_back(c);
    }

    userdata_ptr->flag.store(true);

    return;
}

#if defined(__unix__) || defined(__APPLE__)
extern "C" int main() { program(); }
#elif defined(_WIN32)

#include <windows.h>

#include <memoryapi.h>

#include <winbase.h>

void setup_data() {
    const auto handle = reinterpret_cast<void*>(std::stoll(std::getenv("MAPVIEW_HANDLE")));
    const auto size = std::stoll(std::getenv("MAPVIEW_SIZE"));
    const auto ptr_base = reinterpret_cast<void*>(std::stoll(std::getenv("MAPVIEW_PTR_BASE")));

    printf("hande: %p size: %d ptr_base: %p\n", handle, size, ptr_base);

    auto* ptr_mapped = MapViewOfFileEx(handle, FILE_MAP_ALL_ACCESS, 0, 0, size, ptr_base);
    printf("ptr mapped: %p\n", ptr_mapped);
    if (ptr_mapped == nullptr) {
        throw 0;
    }
}

int main() {
    setup_data();
    program();
}
#endif
