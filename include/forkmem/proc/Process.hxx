
#ifndef FORKMEM_PROCESS_HXX
#define FORKMEM_PROCESS_HXX

#include <filesystem>
#include <string>

namespace forkmem {

struct Executable {
    std::filesystem::path path;
    std::string entry;
};

} // namespace forkmem

#endif