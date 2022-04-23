

#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <string>
#if defined(__linux__) || defined(__APPLE__) || defined(__unix__)

// On unix we are forked from the parent thus already have the memory mapped
void* get_data() {
    const auto user_data_ptr_str = std::getenv("USERDATA_PTR");
    auto sus_pointer_num = std::stol(user_data_ptr_str);
    auto pointer = reinterpret_cast<void*>(sus_pointer_num);
    return pointer;
}
#elif defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
// TODO : lean down the includes
#include <windows.h>

#include <memoryapi.h>

#include <winbase.h>

void* get_data() {
    const auto handle = reinterpret_cast<void*>(std::stoll(std::getenv("MAPVIEW_HANDLE")));
    const auto view_size = std::stoll(std::getenv("MAPVIEW_SIZE"));
    const auto ptr_base = reinterpret_cast<void*>(std::stoll(std::getenv("MAPVIEW_PTR_BASE")));

    const auto userdata_ptr = std::stoll(std::getenv("USERDATA_PTR"));

    auto* ptr_mapped = MapViewOfFileEx(handle, FILE_MAP_ALL_ACCESS, 0, 0, view_size, ptr_base);

    // userdata ptr valid after file map
    return reinterpret_cast<void*>(userdata_ptr);
}

int main() {}

#endif