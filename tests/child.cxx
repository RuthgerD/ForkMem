

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <string>
#include <thread>
#include <forkmem/Memory.hxx>
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

void setup_data() {
#if defined(__unix__) || defined(__APPLE__)
    const auto handle = forkmem::NativeMemory::Handle{
        .handle = std::stoull(std::getenv("MAPVIEW_HANDLE")),
        .memory = reinterpret_cast<void*>(std::stoll(std::getenv("MAPVIEW_PTR_BASE"))),
        .size = std::stoull(std::getenv("MAPVIEW_SIZE")),
    };
    printf("hande: %ld size: %lu ptr_base: %p\n", handle.handle, handle.size, handle.memory);
#elif defined(_WIN32)
    const auto handle = forkmem::NativeMemory::Handle{
        .handle = reinterpret_cast<void*>(std::stoll(std::getenv("MAPVIEW_HANDLE"))),
        .memory = reinterpret_cast<void*>(std::stoll(std::getenv("MAPVIEW_PTR_BASE"))),
        .size = std::stoul(std::getenv("MAPVIEW_SIZE")),
    };
    printf("hande: %p size: %d ptr_base: %p\n", handle.handle, handle.size, handle.memory);
#endif

    forkmem::NativeMemory::make_reachable(handle);
    printf("ptr mapped: %p\n", handle.memory);
    std::fflush(stdout);
}

int main() {
    setup_data();
    program();
}
